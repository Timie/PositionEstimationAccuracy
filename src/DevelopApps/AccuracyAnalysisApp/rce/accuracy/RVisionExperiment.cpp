#include "RVisionExperiment.h"

#include <QSettings>

#include <opencv2/calib3d/calib3d.hpp>

#include "rce/core/RSettingNester.h"
#include "rce/accuracy/RVirtualScene.h"
#include "rce/uncertainty/RUncertaintyGenerator.h"
#include "rce/accuracy/RStatisticsCollector.h"
#include "rce/accuracy/RVirtualCamera.h"
#include "dfs/core/DDebug.h"
#include "dfs/core/DFileDebug.h"
#include "rce/geometry/RGeometry.h"




rce::accuracy::RVisionExperiment::
RVisionExperiment():
    minDataCount_(1),
    avgError_(-1),
    targetsUnder02_(0),
    targetsUnder03_(0),
    targetsUnder04_(0),
    targetsUnder05_(0)
{

}

void
rce::accuracy::RVisionExperiment::
loadSettings(QSettings &settings,
             const QString &sectionName)
{
    rce::core::RSettingNester n(settings,
                                sectionName);

    // load experiment parameters
    if(!settings.contains("experiments_count"))
        settings.setValue("experiments_count",
                          100);
    experimentsCount_ = settings.value("experiments_count").toInt();


    if(!settings.contains("min_data_count_per_target"))
        settings.setValue("min_data_count_per_target",
                          1);
    minDataCount_ = settings.value("min_data_count_per_target").toInt();


    if(!settings.contains("min_target_img_dist_from_img_side"))
        settings.setValue("min_target_img_dist_from_img_side",
                          5);
    minTargetDistFromSide_ = settings.value("min_target_img_dist_from_img_side").toDouble();

    // load scene
    scene_ = RVirtualScene::createFromSettings(settings,
                                           "scene");


    // load uncertainty generators
    tiePointWorldErrorNoiser_ = rce::uncertainty::RUncertaintyGenerator::
                                createFromSettings(settings,
                                                   "tie_point_world_error");
    tiePointImageErrorNoiser_ = rce::uncertainty::RUncertaintyGenerator::
                                createFromSettings(settings,
                                                   "tie_point_image_error");

    tiePointWorldElevationNoiser_ = rce::uncertainty::RUncertaintyGenerator::
                                    createFromSettings(settings,
                                                       "tie_point_elevation_error");
    targetImageErrorNoiser_ = rce::uncertainty::RUncertaintyGenerator::
                        createFromSettings(settings,
                                           "target_image_error");
    targetWorldElevationNoiser_ = rce::uncertainty::RUncertaintyGenerator::
                        createFromSettings(settings,
                                           "target_elevation_error");

    // load statistics collectors
    int numOfTargets = scene_->getTargetsCount();
    targetAvgErrCollectors_.resize(numOfTargets);

    for(int i = 0;
        i < numOfTargets;
        ++i)
    {
        targetAvgErrCollectors_[i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                            "accuracy_collection");

        targetAvgErrCollectors_[i]->setExpectedValue(0);
    }

    targetXCollectors_.resize(numOfTargets);

    for(int i = 0;
        i < numOfTargets;
        ++i)
    {
        targetXCollectors_[i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                            "accuracy_collection");

        targetXCollectors_[i]->setExpectedValue(scene_->getTargets()[i].x);
    }
    targetYCollectors_.resize(numOfTargets);

    for(int i = 0;
        i < numOfTargets;
        ++i)
    {
        targetYCollectors_[i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                            "accuracy_collection");

        targetYCollectors_[i]->setExpectedValue(scene_->getTargets()[i].y);
    }



}

bool
rce::accuracy::RVisionExperiment::
run(const std::vector<uchar> &toCaptureFlags)
{
    if(analyzeNoisefreeScene())
    {
        if(toCaptureFlags.size() > 0)
        {
            assert(toCaptureFlags.size() == targetCaptureFlags_.size());

            for(int i = 0;
                i < targetCaptureFlags_.size();
                ++i)
            {
                targetCaptureFlags_[i] = targetCaptureFlags_[i] && toCaptureFlags[i];
            }
        }

        if(performExperiments(experimentsCount_))
        {
            if(outputFindings())
            {
                return true;
            }
            else
            {
                dWarning() << "RVisionExperiment::run: No targets were captured.";
                return false;
            }
        }
        else
        {
            dWarning() << "RVisionExperiment::run: Not all experiments were finished successfully.";
        }
    }
    else
    {
        dWarning() << "RVisionExperiment::run: Noise-free scene could not be analysed. The set up is probably invalid.";
    }

    return false;
}

bool
rce::accuracy::RVisionExperiment::
analyzeNoisefreeScene()
{
    // capture landmarks
    std::vector<cv::Point2d> capturedLandmarks;
    std::vector<uchar> landmarksFlags;
    scene_->captureLandmarks(capturedLandmarks,
                             landmarksFlags,
                             false);

    int numOfCapturedLandmarks = 0;
    for(int i = 0;
        i < landmarksFlags.size();
        ++i)
    {
        if(landmarksFlags[i] == 0)
        {
            dWarning() << "RVisionExperiment::analyzeNoisefreeScene: Landmark not captured:"
                       << i << scene_->getLandmarks()[i].x << scene_->getLandmarks()[i].y <<
                          "and is projected to" << capturedLandmarks[i].x << capturedLandmarks[i].y;
        }
        else
        {
            dDebug() << "RVisionExperiment::analyzeNoisefreeScene: Landmark captured:"
                       << i << scene_->getLandmarks()[i].x << scene_->getLandmarks()[i].y <<
                          "and is projected to" << capturedLandmarks[i].x << capturedLandmarks[i].y;
            ++numOfCapturedLandmarks;

        }
    }


    // capture targets
    scene_->captureTargets(targetTrueImagePositions_,
                             targetCaptureFlags_,
                           false);

    targetDistFromSide_.resize(targetCaptureFlags_.size());
    for(int i = 0;
        i < targetCaptureFlags_.size();
        ++i)
    {
        if(targetCaptureFlags_[i] == 0)
        {
            dDebug() << "RVisionExperiment::analyzeNoisefreeScene: Target not captured:"
                     << i << scene_->getTargets()[i].x << scene_->getTargets()[i].y;
        }
        else
        {
            targetDistFromSide_[i] = rce::geometry::getDistanceFromImageEdge(targetTrueImagePositions_[i],
                                                                             scene_->getCamera()->getResolution());
        }
    }

    // calculate homography from captured landmarks
    if(numOfCapturedLandmarks < 4)
    {
        dCritical() << "RVisionExperiment::analyzeNoisefreeScene: Not enough captured landmarks for homography estimation" << numOfCapturedLandmarks;
        return false;
    }
    else
    {
        std::vector<cv::Point2d> validCapturedLandmarks;
        extractValidElements(capturedLandmarks,
                             landmarksFlags,
                             validCapturedLandmarks);
        std::vector<cv::Point2d> validSceneLandmarks;
        extractValidElements(scene_->getLandmarks(),
                             landmarksFlags,
                             validSceneLandmarks);

        // estimate homography
        cv::Mat homography = cv::findHomography(validCapturedLandmarks,
                           validSceneLandmarks);

        // so, we got the homography... now see, how well we can estimate points with it...
        rce::accuracy::RStatisticsCollector errCollector;
        targetsNumericErrors_.clear();
        targetsNumericErrors_.resize(scene_->getTargetsCount(),
                                     -1);
        for(int i = 0;
            i < targetTrueImagePositions_.size();
            ++i)
        {
            if(targetCaptureFlags_[i] != 0)
            {
                cv::Vec3d imageTargetVecd(targetTrueImagePositions_[i].x,
                                        targetTrueImagePositions_[i].y,
                                          1);
                cv::Vec3d estimatedTargetVec = cv::Mat(homography * cv::Mat(imageTargetVecd));
                estimatedTargetVec = estimatedTargetVec * (1.0  / estimatedTargetVec(2));

                double err = cv::norm(cv::Point2d(scene_->getTargets()[i].x - estimatedTargetVec(0),
                                                  scene_->getTargets()[i].y - estimatedTargetVec(1)));
                dInfo() << "RVisionExperiment::analyzeNoisefreeScene: Error of target"
                         << i << " is " << err;
                errCollector.addDatum(err);
                targetsNumericErrors_[i] = err;
            }
        }

        dDebug() << "RVisionExperiment::analyzeNoisefreeScene: Target errors has following nature: " << errCollector.getReadableReport();
        return true;
    }

}

bool
rce::accuracy::RVisionExperiment::
performSingleExperiment()
{
    // create scene which is noisy...
    std::vector<cv::Point2d> noisyLandmarks;
    noisyLandmarks.reserve(scene_->getLandmarksCount());
    std::vector<double> landmarkElevErrors;
    landmarkElevErrors.reserve(scene_->getLandmarksCount());
    for(size_t i = 0;
        i < scene_->getLandmarks().size();
        ++i)
    {
        noisyLandmarks.push_back(cv::Point2d(tiePointWorldErrorNoiser_->generateValue(scene_->getLandmarks()[i].x),
                                             tiePointWorldErrorNoiser_->generateValue(scene_->getLandmarks()[i].y)));
        landmarkElevErrors.push_back(tiePointWorldElevationNoiser_->generateValue(0));
    }

    std::vector<double> targetElevErrors;
    targetElevErrors.reserve(scene_->getTargets().size());
    for(size_t i = 0;
        i < scene_->getTargets().size();
        ++i)
    {
        targetElevErrors.push_back(targetWorldElevationNoiser_->generateValue(0));
    }

    rce::accuracy::RVirtualScene noisyScene(scene_->getCamera()->cloneShared(),
                                            scene_->getPointOfView(),
                                            noisyLandmarks,
                                            scene_->getTargets(),
                                            landmarkElevErrors,
                                            targetElevErrors);

    // capture landmarks
    std::vector<cv::Point2d> capturedLandmarks;
    std::vector<uchar> landmarksFlags;
    noisyScene.captureLandmarks(capturedLandmarks,
                             landmarksFlags);

    int numOfCapturedLandmarks = 0;
    for(int i = 0;
        i < landmarksFlags.size();
        ++i)
    {
        if(landmarksFlags[i] == 0)
        {
            dWarning() << "RVisionExperiment::performSingleExperiment: Landmark not captured:"
                       << i << noisyScene.getLandmarks()[i].x << noisyScene.getLandmarks()[i].y;
        }
        else
        {
            capturedLandmarks[i].x = tiePointImageErrorNoiser_->generateValue(capturedLandmarks[i].x);
            capturedLandmarks[i].y = tiePointImageErrorNoiser_->generateValue(capturedLandmarks[i].y);
            ++numOfCapturedLandmarks;
        }
    }


    // capture targets
    std::vector<cv::Point2d> capturedTargets;
    std::vector<uchar> targetsFlags;
    noisyScene.captureTargets(capturedTargets,
                             targetsFlags,
                              true,
                              targetCaptureFlags_);

    for(int i = 0;
        i < targetsFlags.size();
        ++i)
    {
        if(targetsFlags[i] == 0)
        {
           // dDebug() << "RVisionExperiment::performSingleExperiment: Target not captured:"
           //          << i << noisyScene.getTargets()[i].x << noisyScene.getTargets()[i].y;
        }
        else
        {
            capturedTargets[i].x = targetImageErrorNoiser_->generateValue(capturedTargets[i].x);
            capturedTargets[i].y = targetImageErrorNoiser_->generateValue(capturedTargets[i].y);
        }
    }

    // calculate homography from captured landmarks
    if(numOfCapturedLandmarks < 4)
    {
        dCritical() << "RVisionExperiment::performSingleExperiment: Not enough captured landmarks for homography estimation" << numOfCapturedLandmarks;
        return false;
    }
    else
    {
        std::vector<cv::Point2d> validCapturedLandmarks;
        extractValidElements(capturedLandmarks,
                             landmarksFlags,
                             validCapturedLandmarks);
        std::vector<cv::Point2d> validSceneLandmarks;
        extractValidElements(scene_->getLandmarks(),
                             landmarksFlags,
                             validSceneLandmarks);

        // estimate homography
        cv::Mat homography = cv::findHomography(validCapturedLandmarks,
                           validSceneLandmarks);

        rce::accuracy::RStatisticsCollector errCollector;
        for(int i = 0;
            i < capturedTargets.size();
            ++i)
        {
            if(targetsFlags[i] != 0)
            {
                cv::Vec3d imageTargetVecd(capturedTargets[i].x,
                                        capturedTargets[i].y,
                                          1);
                cv::Vec3d estimatedTargetVec = cv::Mat(homography * cv::Mat(imageTargetVecd));
                estimatedTargetVec = estimatedTargetVec * (1.0  / estimatedTargetVec(2));

                double err = cv::norm(cv::Point2d(scene_->getTargets()[i].x - estimatedTargetVec(0),
                                                  scene_->getTargets()[i].y - estimatedTargetVec(1)));
                dInfo() << "RVisionExperiment::performSingleExperiment: Error of target"
                         << i << " is " << err;
                errCollector.addDatum(err);
                targetAvgErrCollectors_[i]->addDatum(err);
                targetXCollectors_[i]->addDatum(estimatedTargetVec(0));
                targetYCollectors_[i]->addDatum(estimatedTargetVec(1));
            }
        }

        dDebug() << "RVisionExperiment::performSingleExperiment: Target errors has following nature: " << errCollector.getReadableReport();
        return true;
    }
}

bool
rce::accuracy::RVisionExperiment::
performExperiments(int numOfExperiments)
{
    bool allSuccessFlag = true;
    for(int i = 0;
        i < numOfExperiments;
        ++i)
    {
        if(!performSingleExperiment())
        {
            allSuccessFlag = false;
        }
    }

    return allSuccessFlag;
}

bool
rce::accuracy::RVisionExperiment::
outputFindings()
{
    avgError_ = 0;
    targetsUnder02_= (0);
    targetsUnder03_ = (0);
    targetsUnder04_ = (0);
    targetsUnder05_ = (0);

    // output accuracy stuff
    bool hasTargetsCaptured = false;
    for(int i = 0;
        i < targetXCollectors_.size();
        ++i)
    {
        if(targetXCollectors_[i]->getDataCount() > minDataCount_)
        {
            hasTargetsCaptured = true;
            dInfo() << "Target X estimated values for target" << i << "is" << targetXCollectors_[i]->getReadableReport() <<
                        "while the true value is" << scene_->getTargets()[i].x;
            dInfo() << "Target Y estimated values for target" << i << "is" << targetYCollectors_[i]->getReadableReport() <<
                        "while the true value is" << scene_->getTargets()[i].y;
            dInfo() << "Target average error nature for target" << i << "is" << targetAvgErrCollectors_[i]->getReadableReport() <<
                        "while numeric error may account for" << targetsNumericErrors_[i];
        }

    }
    if(!hasTargetsCaptured)
    {
        return false;
    }


    QVector<cv::Point3d> accuracySceneVector;
    QVector<cv::Point3d> accuracyImageVector;



    // output findings of error distribution (just a test)
    cv::Point2d imgCentre = scene_->getCamera()->getPrincipalPoint();
    cv::Point2d landmarkCentre = scene_->calculateLandmarkCentre();

    double avgErrorOfTargets = 0; // distance from true position
    int counter = 0;
    double avgMeanSigmaXOfTargets = 0; // distance from average position
    double avgMeanSigmaYOfTargets = 0; // distance from average position
    double avgMeanSigmaOfTargets = 0;
    double avgTrueSigmaXOfTargets = 0; // distance from true position
    double avgTrueSigmaYOfTargets = 0; // distance from true position
    double avgTrueSigmaOfTargets = 0;
    // double is distance, touple is: number of points, avg error, sigma from mean, sigma from true value
    std::map<double,std::tuple<int,double,double,double>> avgErrorFromImageCentre; // distance -> pair(avg error, mean sigma, true sigma)
    std::map<double,std::tuple<int,double,double,double>> avgErrorFromLandmarkCentre; // distance -> pair(avg error, mean sigma, true sigma)

    for(int i = 0;
        i < scene_->getTargetsCount();
        ++i)
    {
        if((targetCaptureFlags_[i] != 0) &&
           (targetAvgErrCollectors_[i]->getDataCount() > minDataCount_) &&
           (targetDistFromSide_[i] > minTargetDistFromSide_))
        {
            counter++;

            double error = targetAvgErrCollectors_[i]->getMean();
            double meanSigmaX = targetXCollectors_[i]->getSigma();
            double meanSigmaY = targetYCollectors_[i]->getSigma();
            double meanSigma = std::sqrt(meanSigmaX * meanSigmaX + meanSigmaY * meanSigmaY);

            double trueSigmaX = targetXCollectors_[i]->getDeviationFromExpectedValue();
            double trueSigmaY = targetYCollectors_[i]->getDeviationFromExpectedValue();
            double trueSigma = std::sqrt(trueSigmaX * trueSigmaX + trueSigmaY * trueSigmaY);
            double distFromImgCentre = cv::norm(targetTrueImagePositions_[i] - imgCentre);
            double distFromLandmarkCentre = cv::norm(scene_->getTargets()[i] - landmarkCentre);
            if(std::isnan(error + meanSigmaX + meanSigmaY + meanSigma + distFromImgCentre + distFromLandmarkCentre))
            {
                dDebug() << "Uncertainty Measurement for target" << i << "is NaN:" <<
                            error << meanSigmaX << meanSigmaY << meanSigma << distFromImgCentre << distFromLandmarkCentre;
            }
            avgErrorOfTargets += error;
            avgMeanSigmaXOfTargets += meanSigmaX;
            avgMeanSigmaYOfTargets += meanSigmaY;
            avgMeanSigmaOfTargets += meanSigma;


            avgTrueSigmaXOfTargets += trueSigmaX;
            avgTrueSigmaYOfTargets += trueSigmaY;
            avgTrueSigmaOfTargets += trueSigma;

            addToAvgMap(avgErrorFromImageCentre,
                        qRound64(distFromImgCentre * 10) / 10.0,
                        error,
                        meanSigma,
                        trueSigma);

            addToAvgMap(avgErrorFromLandmarkCentre,
                        qRound64(distFromLandmarkCentre * 10) / 10.0,
                        error,
                        meanSigma,
                        trueSigma);

            accuracySceneVector.push_back(cv::Point3d(scene_->getTargets()[i].x,
                                                      scene_->getTargets()[i].y,
                                                      error));
            accuracyImageVector.push_back(cv::Point3d(targetTrueImagePositions_[i].x,
                                                      targetTrueImagePositions_[i].y,
                                                      error));

            if(error < 0.2)
            {
                ++targetsUnder02_;
            }
            if(error < 0.3)
            {
                ++targetsUnder03_;
            }
            if(error < 0.4)
            {
                ++targetsUnder04_;
            }
            if(error < 0.5)
            {
                ++targetsUnder05_;
            }

        }
        else
        {
            accuracySceneVector.push_back(cv::Point3d(scene_->getTargets()[i].x,
                                                      scene_->getTargets()[i].y,
                                                      std::numeric_limits<double>::quiet_NaN()));

            accuracyImageVector.push_back(cv::Point3d(targetTrueImagePositions_[i].x,
                                                      targetTrueImagePositions_[i].y,
                                                      std::numeric_limits<double>::quiet_NaN()));
        }
    }

    // output accuracy vectors
#pragma omp critical
    {

        dFileOutput(QString("AvgErrorSpatiallyInScene_%1.txt").arg(idString_))
                 << vector3dToCSV(accuracySceneVector);

        dFileOutput(QString("AvgErrorSpatiallyInImage_%1.txt").arg(idString_))
                 << vector3dToCSV(accuracyImageVector);


        dFileOutput(QString("AvgErrorAndSigmaFromImageCentre_%1.txt").arg(idString_))
                << avgMapToStringCSV(avgErrorFromImageCentre);


        dFileOutput(QString("AvgErrorAndSigmaFromLandmarkCentre_%1.txt").arg(idString_))
                << avgMapToStringCSV(avgErrorFromLandmarkCentre);

        dFileOutput(QString("AvgError_%1.txt").arg(idString_))  << "AvgError"  << avgErrorOfTargets / counter << "\n" <<
                    "AvgMeanSigmaX" << avgMeanSigmaXOfTargets / counter <<"\n" <<
                    "AvgMeanSigmaY" << avgMeanSigmaYOfTargets / counter <<"\n" <<
                    "AvgMeanSigma" << avgMeanSigmaOfTargets / counter <<"\n" <<
                    "AvgTrueSigmaX" << avgTrueSigmaXOfTargets / counter <<"\n" <<
                    "AvgTrueSigmaY" << avgTrueSigmaYOfTargets / counter <<"\n" <<
                    "AvgTrueSigma" << avgTrueSigmaOfTargets / counter;
    }

    avgError_ = avgErrorOfTargets / counter;

    return true;
}

void
rce::accuracy::RVisionExperiment::
addToAvgMap(std::map<double, std::tuple<int, double, double, double> > &avgMap,
            double key,
            double error,
            double meanSigma,
            double trueSigma)
{
    std::tuple<int, double, double,double> &data = avgMap[key];
    ++(std::get<0>(data));
    (std::get<1>(data)) += error;
    (std::get<2>(data)) += meanSigma;
    (std::get<3>(data)) += trueSigma;
}

QString
rce::accuracy::RVisionExperiment::
avgMapToStringCSV(const std::map<double, std::tuple<int, double, double,double> > &avgMap)
{
    QString result;
    for(auto it = avgMap.begin();
        it != avgMap.end();
        ++it)
    {
        const std::tuple<int, double, double,double> &data = it->second;
        result += QString("%1; %2; %3; %4;\n")
                  .arg(it->first)
                  .arg(std::get<1>(data) / std::get<0>(data))
                  .arg(std::get<2>(data) / std::get<0>(data))
                  .arg(std::get<3>(data) / std::get<0>(data));

    }
    return result;
}

QString
rce::accuracy::RVisionExperiment::
vector3dToCSV(const QVector<cv::Point3d> &vec)
{
    QString result;
    for(int i = 0;
        i < vec.size();
        ++i)
    {
        result.append(QString("%1;%2;%3;\n")
                      .arg(vec[i].x, 0, 'f', 13)
                      .arg(vec[i].y, 0, 'f', 13)
                      .arg(vec[i].z, 0, 'f', 13));
    }

    return result;
}


template <typename T>
void
rce::accuracy::RVisionExperiment::
extractValidElements(const std::vector<T> &allVec,
                     const std::vector<uchar> &flags,
                     std::vector<T> &extractedVec)
{
    assert(allVec.size() == flags.size());
    extractedVec.clear();
    extractedVec.reserve(allVec.size());
    for(size_t i = 0;
        i < flags.size();
        ++i)
    {
        if(flags[i] != 0)
        {
            extractedVec.push_back(allVec[i]);
        }
    }
}

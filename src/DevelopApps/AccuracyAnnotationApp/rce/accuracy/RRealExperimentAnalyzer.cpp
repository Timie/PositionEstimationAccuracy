#include "RRealExperimentAnalyzer.h"

#include <QSettings>
#include <QStringList>
#include <QVector>
#include <QSet>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "rce/core/RSettingNester.h"
#include "rce/accuracy/RStatisticsCollector.h"
#include "rce/geometry/RCameraPoseEstimation.h"
#include "rce/geometry/RGeometry.h"
#include "dfs/core/DDebug.h"
#include "dfs/core/DFileDebug.h"
#include "rce/utility/ROpenCVtoQDebug.h"

#define RCE_SHOW_VISUALISATION (false)

#define RCE_MIN_OBSERS_PER_LANDMARK (1)
#define RCE_MIN_OBSERS_PER_IMAGE (1)
#define RCE_MIN_OBSERS_GLOBALLY (1)

rce::accuracy::RRealExperimentAnalyzer::
RRealExperimentAnalyzer():
    randomLandmarks_(false)
{

}

void
rce::accuracy::RRealExperimentAnalyzer::
loadSettings(QSettings &settings, const QString &sectionName)
{
    rce::core::RSettingNester n(settings,
                                sectionName);


    if(!settings.contains("fx"))
        settings.setValue("fx",
                          960);
    fx_ = settings.value("fx").toDouble();
    if(!settings.contains("fy"))
        settings.setValue("fy",
                          960);
    fy_ = settings.value("fy").toDouble();

    if(!settings.contains("principal_point_x"))
        settings.setValue("principal_point_x",
                          960);
    principalPoint_.x = settings.value("principal_point_x").toDouble();
    if(!settings.contains("principal_point_y"))
        settings.setValue("principal_point_y",
                          540);
    principalPoint_.y = settings.value("principal_point_y").toDouble();

    if(!settings.contains("random_landmarks"))
        settings.setValue("random_landmarks",
                          "false");
    randomLandmarks_ = (settings.value("random_landmarks").toString().toLower() == "true");

    if(randomLandmarks_)
    {
        if(!settings.contains("num_of_random_trials"))
            settings.setValue("num_of_random_trials",
                              10);
        numOfRandomTrials_ =  settings.value("num_of_random_trials").toInt();

        landmarksIndices_.clear();
        for(int i = 0;
            i < numOfRandomTrials_;
            ++i)
        {
            bool done = false;
            while(!done)
            {
                std::vector<int> indices;
                for(int j = 0; j < 4; ++j)
                {
                    indices.push_back(rand() % worldPoints_.size());
                }

                if(allElementsUnique(indices))
                {
                    landmarksIndices_.push_back(indices);
                    done = true;
                }
            }
        }
    }
    else
    {
        landmarksIndices_.clear();
        if(!settings.contains("landmarks_indices"))
            settings.setValue("landmarks_indices",
                              "0,1,2,3");
        QStringList landIndicesListStr = settings.value("landmarks_indices").toString().split(QChar(';'),QString::SkipEmptyParts);
        landmarksIndices_.resize(landIndicesListStr.size());
        for(int i = 0;
            i < landIndicesListStr.size();
            ++i)
        {
            QStringList landIndicesStr = landIndicesListStr[i].split(QChar(','),QString::SkipEmptyParts);
            for(int j = 0;
                j < landIndicesStr.size();
                ++j)
            {
                landmarksIndices_[i].push_back(landIndicesStr[j].toInt());
            }

        }
    }


    int numOfTargets =  worldPoints_.size();
    targetAvgErrCollectorsPerLandmark_.resize(landmarksIndices_.size());

    for(int lmkCfgIdx = 0;
        lmkCfgIdx < landmarksIndices_.size();
        ++lmkCfgIdx)
    {
        targetAvgErrCollectorsPerLandmark_[lmkCfgIdx].resize(numOfTargets);
        for(int i = 0;
            i < numOfTargets;
            ++i)
        {
            targetAvgErrCollectorsPerLandmark_[lmkCfgIdx][i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                                "accuracy_collection");

            targetAvgErrCollectorsPerLandmark_[lmkCfgIdx][i]->setExpectedValue(0);
        }
    }

    targetXCollectorsPerLandmark_.resize(landmarksIndices_.size());
    for(int lmkCfgIdx = 0;
        lmkCfgIdx < landmarksIndices_.size();
        ++lmkCfgIdx)
    {
        targetXCollectorsPerLandmark_[lmkCfgIdx].resize(numOfTargets);

        for(int i = 0;
            i < numOfTargets;
            ++i)
        {
            targetXCollectorsPerLandmark_[lmkCfgIdx][i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                                                  "accuracy_collection");

            targetXCollectorsPerLandmark_[lmkCfgIdx][i]->setExpectedValue(worldPoints_[i].x);
        }
    }
    targetYCollectorsPerLandmark_.resize(landmarksIndices_.size());
    for(int lmkCfgIdx = 0;
        lmkCfgIdx < landmarksIndices_.size();
        ++lmkCfgIdx)
    {
        targetYCollectorsPerLandmark_[lmkCfgIdx].resize(numOfTargets);

        for(int i = 0;
            i < numOfTargets;
            ++i)
        {
            targetYCollectorsPerLandmark_[lmkCfgIdx][i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                                "accuracy_collection");

            targetYCollectorsPerLandmark_[lmkCfgIdx][i]->setExpectedValue(worldPoints_[i].y);
        }
    }


    globTargetAvgErrCollectors_.resize(numOfTargets);

    for(int i = 0;
        i < numOfTargets;
        ++i)
    {
        globTargetAvgErrCollectors_[i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                            "accuracy_collection");

        globTargetAvgErrCollectors_[i]->setExpectedValue(0);
    }

    globTargetXCollectors_.resize(numOfTargets);

    for(int i = 0;
        i < numOfTargets;
        ++i)
    {
        globTargetXCollectors_[i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                            "accuracy_collection");

        globTargetXCollectors_[i]->setExpectedValue(worldPoints_[i].x);
    }
    globTargetYCollectors_.resize(numOfTargets);

    for(int i = 0;
        i < numOfTargets;
        ++i)
    {
        globTargetYCollectors_[i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                            "accuracy_collection");

        globTargetYCollectors_[i]->setExpectedValue(worldPoints_[i].y);
    }

    targetAvgErrCollectorsPerImage_.resize(imagePoints_.size());
    targetXCollectorsPerImage_.resize(imagePoints_.size());
    targetYCollectorsPerImage_.resize(imagePoints_.size());
    for(int imgIdx = 0;
        imgIdx < imagePoints_.size();
        ++imgIdx)
    {
        targetAvgErrCollectorsPerImage_[imgIdx].resize(numOfTargets);

        for(int i = 0;
            i < numOfTargets;
            ++i)
        {
            targetAvgErrCollectorsPerImage_[imgIdx][i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                                "accuracy_collection");

            targetAvgErrCollectorsPerImage_[imgIdx][i]->setExpectedValue(0);
        }

        targetXCollectorsPerImage_[imgIdx].resize(numOfTargets);

        for(int i = 0;
            i < numOfTargets;
            ++i)
        {
            targetXCollectorsPerImage_[imgIdx][i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                                "accuracy_collection");

            targetXCollectorsPerImage_[imgIdx][i]->setExpectedValue(worldPoints_[i].x);
        }
        targetYCollectorsPerImage_[imgIdx].resize(numOfTargets);

        for(int i = 0;
            i < numOfTargets;
            ++i)
        {
            targetYCollectorsPerImage_[imgIdx][i] = rce::accuracy::RStatisticsCollector::createFromSettings(settings,
                                                                                                "accuracy_collection");

            targetYCollectorsPerImage_[imgIdx][i]->setExpectedValue(worldPoints_[i].y);
        }
    }


}

void
rce::accuracy::RRealExperimentAnalyzer::
setCameraParameters(const cv::Mat &camMatrix)
{
    if(!camMatrix.empty())
    {
        fx_ = camMatrix.at<double>(0,0);
        fy_ = camMatrix.at<double>(1,1);
        principalPoint_.x = camMatrix.at<double>(0,2);
        principalPoint_.y = camMatrix.at<double>(1,2);
    }
}

void
rce::accuracy::RRealExperimentAnalyzer::
setData(const std::vector<cv::Point2d> &worldPoints,
        const std::vector<std::vector<cv::Point2d> > &imagePoints,
        const std::vector<std::vector<uchar> > &annotFlags,
        const QVector<cv::Mat> &images)
{
    worldPoints_ = worldPoints;
    imagePoints_ = imagePoints;
    annotationFlags_ = annotFlags;
    images_ = images;
}

void
rce::accuracy::RRealExperimentAnalyzer::
performAnalysis()
{
    // firstly, print out camera positions:
    for(int i = 0;
        i < imagePoints_.size();
        ++i)
    {
        dDebug() << "Estimated camera positions for image" << i;
        // FIXME: Pick only valid pairs
        std::vector<cv::Point2d> selectedWorldPoints;
        std::vector<cv::Point2d> selectedImagePoints;
        for(int j = 0;
            j < annotationFlags_[i].size();
            ++j)
        {
            if(annotationFlags_[i][j] != 0)
            {
                selectedWorldPoints.push_back(worldPoints_[j]);
                selectedImagePoints.push_back(imagePoints_[i][j]);
            }
        }
        estimateCameraPosition(selectedWorldPoints,
                               selectedImagePoints,
                               i);
    }

    // prepare global data collectors

    std::map<double,std::tuple<int,double,double,double>> avgErrorFromLandmarkCentre; // distance -> pair(avg error, mean sigma, true sigma)
    std::map<double,std::tuple<int,double,double,double>> avgErrorFromImageCentre; // distance -> pair(avg error, mean sigma, true sigma)
    // prepare true positions
    for(int i = 0;
        i < worldPoints_.size();
        ++i)
    {
        globTargetXCollectors_[i]->setExpectedValue(worldPoints_[i].x);
        globTargetYCollectors_[i]->setExpectedValue(worldPoints_[i].y);
        globTargetAvgErrCollectors_[i]->setExpectedValue(0); // FIXME: Add setting expected value for image collectors

        for(int imgIdx = 0;
            imgIdx < imagePoints_.size();
            ++imgIdx)
        {
            targetXCollectorsPerImage_[imgIdx][i]->setExpectedValue(worldPoints_[i].x);
            targetYCollectorsPerImage_[imgIdx][i]->setExpectedValue(worldPoints_[i].y);
            targetAvgErrCollectorsPerImage_[imgIdx][i]->setExpectedValue(0);
        }
    }

    // for every landmark configuration...
    for(int lndCfgIdx = 0;
        lndCfgIdx < landmarksIndices_.size();
        ++lndCfgIdx)
    {
        QVector<int> landmarkIndices = QVector<int>::fromStdVector(landmarksIndices_[lndCfgIdx]);
        dDebug() << "Landmark configuration:" << lndCfgIdx << ":" << landmarkIndices;
        QSet<int> usedIndices = QSet<int>::fromList(landmarkIndices.toList());

        // prepare avg err collectors for this configuration
        for(int i = 0;
            i < worldPoints_.size();
            ++i)
        {
            targetAvgErrCollectorsPerLandmark_[lndCfgIdx][i]->clearData();
            targetXCollectorsPerLandmark_[lndCfgIdx][i]->clearData();
            targetYCollectorsPerLandmark_[lndCfgIdx][i]->clearData();

            targetAvgErrCollectorsPerLandmark_[lndCfgIdx][i]->setExpectedValue(0);
            targetXCollectorsPerLandmark_[lndCfgIdx][i]->setExpectedValue(worldPoints_[i].x);
            targetYCollectorsPerLandmark_[lndCfgIdx][i]->setExpectedValue(worldPoints_[i].y);
        }


        // extract landmarks in world

        std::vector<cv::Point2d> landmarksWorld;
        for(int indIdx = 0;
            indIdx < landmarkIndices.size();
            ++indIdx)
        {
            landmarksWorld.push_back(worldPoints_[landmarkIndices[indIdx]]);
        }

        // output landmarks stuff and target positions
        if(lndCfgIdx == 0)
        {
            QString landmarkString = QString("landmarks\\size=%1\n").arg(landmarksWorld.size());
            for(int indIdx = 0;
                indIdx < landmarkIndices.size();
                ++indIdx)
            {
                landmarkString += (QString("landmarks\\%1\\landmark_x=%2\n"
                                         "landmarks\\%1\\landmark_y=%3\n")
                                 .arg(indIdx + 1)
                                 .arg(landmarksWorld[indIdx].x, 0, 'f', 17)
                                 .arg(landmarksWorld[indIdx].y, 0, 'f', 17));
            }

            dFileOutput("config_for_simulation.ini") << "\n" << landmarkString;

            // target positions
            QString targetString = QString("targets_definition=list\n"
                                           "targets_list\\size=%1\n")
                                   .arg(worldPoints_.size());

            for(int tgtIdx = 0;
                tgtIdx < worldPoints_.size();
                ++tgtIdx)
            {
                targetString += (QString("targets_list\\%1\\target_x=%2\n"
                                         "targets_list\\%1\\target_y=%3\n")
                                 .arg(tgtIdx + 1)
                                 .arg(worldPoints_[tgtIdx].x, 0, 'f', 17)
                                 .arg(worldPoints_[tgtIdx].y, 0, 'f', 17));


            }

            dFileOutput("config_for_simulation.ini") << "\n" << targetString;
        }


        cv::Point2d landmarkCentre = calculateLandmarkCentre(landmarksWorld);



        // go along every image...
        for(int imgIdx = 0;
            imgIdx < imagePoints_.size();
            ++imgIdx)
        {
            // prepare valid flags
            std::vector<uchar> validPoints = annotationFlags_[imgIdx];
            for(int ptIdx = 0;
                ptIdx < validPoints.size();
                ++ptIdx)
            {
                if(usedIndices.contains(ptIdx))
                {
                    validPoints[ptIdx] = 0;
                }
            }
            // extract landmark images
            std::vector<cv::Point2d> landmarkImages;
            for(int lndIdx = 0;
                lndIdx < landmarkIndices.size();
                ++lndIdx)
            {
                if(annotationFlags_[imgIdx][landmarkIndices[lndIdx]] != 0)
                {
                    landmarkImages.push_back(imagePoints_[imgIdx][landmarkIndices[lndIdx]]);
                }
            }

            if(landmarkImages.size() < 4)
            {
                dCritical() << "Unsufficient number of landmarks";
            }
            else
            {
                // extract target points and imagesimages
                std::vector<int> targetIndices;
                std::vector<cv::Point2d> targetImagePoints;
                std::vector<cv::Point2d> targetWorldPoints;
                for(int k = 0;
                    k < validPoints.size();
                    ++k)
                {
                    if(validPoints[k] != 0)
                    {
                        targetIndices.push_back(k);
                        targetImagePoints.push_back(imagePoints_[imgIdx][k]);
                        targetWorldPoints.push_back(worldPoints_[k]);
                    }
                }

                // calculate homography
                cv::Mat homography = cv::findHomography(landmarkImages,
                                   landmarksWorld);

                if(homography.empty())
                {
                    dCritical() << "Failed to calcualte homography";
                }
                else
                {
                    rce::accuracy::RStatisticsCollector errCollector;
//                    cv::Mat vis;
//                    if(RCE_SHOW_VISUALISATION)
//                    {
//                        vis = images_[imgIdx].clone();
//                    }

                    for(int tgtIdx = 0;
                        tgtIdx < targetIndices.size();
                        ++tgtIdx)
                    {
                        cv::Vec3d imageTargetVecd(targetImagePoints[tgtIdx].x,
                                                  targetImagePoints[tgtIdx].y,
                                                  1);
                        cv::Vec3d estimatedTargetVec = cv::Mat(homography * cv::Mat(imageTargetVecd));
                        estimatedTargetVec = estimatedTargetVec * (1.0  / estimatedTargetVec(2));

                        double err = cv::norm(cv::Point2d(targetWorldPoints[tgtIdx].x - estimatedTargetVec(0),
                                                          targetWorldPoints[tgtIdx].y - estimatedTargetVec(1)));
                        if(std::isnan(err))
                        {
                            qDebug() << "We have NAN";
                        }
                        dInfo() << "RVisionExperiment::performSingleExperiment: Error of target"
                                << targetIndices[tgtIdx] << " is " << err;
                        errCollector.addDatum(err);
                        targetAvgErrCollectorsPerLandmark_[lndCfgIdx][targetIndices[tgtIdx]]->addDatum(err);
                        targetXCollectorsPerLandmark_[lndCfgIdx][targetIndices[tgtIdx]]->addDatum(estimatedTargetVec(0));
                        targetYCollectorsPerLandmark_[lndCfgIdx][targetIndices[tgtIdx]]->addDatum(estimatedTargetVec(1));

                        globTargetAvgErrCollectors_[targetIndices[tgtIdx]]->addDatum(err);
                        globTargetXCollectors_[targetIndices[tgtIdx]]->addDatum(estimatedTargetVec(0));
                        globTargetYCollectors_[targetIndices[tgtIdx]]->addDatum(estimatedTargetVec(1));

                        targetAvgErrCollectorsPerImage_[imgIdx][targetIndices[tgtIdx]]->addDatum(err);
                        targetXCollectorsPerImage_[imgIdx][targetIndices[tgtIdx]]->addDatum(estimatedTargetVec(0));
                        targetYCollectorsPerImage_[imgIdx][targetIndices[tgtIdx]]->addDatum(estimatedTargetVec(1));
                    }

                    dInfo() << "RVisionExperiment::performSingleExperiment: Target errors has following nature: " << errCollector.getReadableReport();
                }
            }

        }

        // prepare accuracy stuff tied to landmark positions
        for(int j = 0;
            j < worldPoints_.size();
            ++j)
        {
            dInfo() << "Target X estimated values for target" << j << "is" << targetXCollectorsPerLandmark_[lndCfgIdx][j]->getReadableReport() <<
                        "while the true value is" << worldPoints_[j].x;
            dInfo() << "Target Y estimated values for target" << j << "is" << targetYCollectorsPerLandmark_[lndCfgIdx][j]->getReadableReport() <<
                        "while the true value is" << worldPoints_[j].y;
            dInfo() << "Target average error nature for target" << j << "is" << targetAvgErrCollectorsPerLandmark_[lndCfgIdx][j]->getReadableReport();

            if(targetAvgErrCollectorsPerLandmark_[lndCfgIdx][j]->getDataCount() >= RCE_MIN_OBSERS_PER_LANDMARK)
            {
                double error = targetAvgErrCollectorsPerLandmark_[lndCfgIdx][j]->getMean();
                double meanSigmaX = targetXCollectorsPerLandmark_[lndCfgIdx][j]->getSigma();
                double meanSigmaY = targetYCollectorsPerLandmark_[lndCfgIdx][j]->getSigma();
                double meanSigma = std::sqrt(meanSigmaX * meanSigmaX + meanSigmaY * meanSigmaY);

                double trueSigmaX = targetXCollectorsPerLandmark_[lndCfgIdx][j]->getDeviationFromExpectedValue();
                double trueSigmaY = targetYCollectorsPerLandmark_[lndCfgIdx][j]->getDeviationFromExpectedValue();
                double trueSigma = std::sqrt(trueSigmaX * trueSigmaX + trueSigmaY * trueSigmaY);

                double distFromLandmarkCentre = cv::norm(worldPoints_[j] - landmarkCentre);
                addToAvgMap(avgErrorFromLandmarkCentre,
                            distFromLandmarkCentre,
                            error,
                            meanSigma,
                            trueSigma);
            }

        }

    }

    // prepare accuracy stuff tied to images
    cv::Point2d imgCentre = principalPoint_;
    for(int imgIdx = 0;
        imgIdx < imagePoints_.size();
        ++imgIdx)
    {

        QVector<cv::Point3d> accuracyImageVector;

        for(int j = 0;
            j < worldPoints_.size();
            ++j)
        {
            dInfo() << "Target X estimated values for target" << j << "is" << targetXCollectorsPerImage_[imgIdx][j]->getReadableReport() <<
                        "while the true value is" << worldPoints_[j].x;
            dInfo() << "Target Y estimated values for target" << j << "is" << targetYCollectorsPerImage_[imgIdx][j]->getReadableReport() <<
                        "while the true value is" << worldPoints_[j].y;
            dInfo() << "Target average error nature for target" << j << "is" << targetAvgErrCollectorsPerImage_[imgIdx][j]->getReadableReport();

            if(targetXCollectorsPerImage_[imgIdx][j]->getDataCount() >= RCE_MIN_OBSERS_PER_IMAGE)
            {
                double error = targetAvgErrCollectorsPerImage_[imgIdx][j]->getMean();
                double meanSigmaX = targetXCollectorsPerImage_[imgIdx][j]->getSigma();
                double meanSigmaY = targetYCollectorsPerImage_[imgIdx][j]->getSigma();
                double meanSigma = std::sqrt(meanSigmaX * meanSigmaX + meanSigmaY * meanSigmaY);

                double trueSigmaX = targetXCollectorsPerImage_[imgIdx][j]->getDeviationFromExpectedValue();
                double trueSigmaY = targetYCollectorsPerImage_[imgIdx][j]->getDeviationFromExpectedValue();
                double trueSigma = std::sqrt(trueSigmaX * trueSigmaX + trueSigmaY * trueSigmaY);


                double distFromImageCentre = cv::norm(imagePoints_[imgIdx][j] - imgCentre);
                addToAvgMap(avgErrorFromImageCentre,
                            distFromImageCentre,
                            error,
                            meanSigma,
                            trueSigma);


                accuracyImageVector.push_back(cv::Point3d(imagePoints_[imgIdx][j].x,
                                                          imagePoints_[imgIdx][j].y,
                                                          error));
            }
            else
            {

                accuracyImageVector.push_back(cv::Point3d(imagePoints_[imgIdx][j].x,
                                                          imagePoints_[imgIdx][j].y,
                                                          std::numeric_limits<double>::quiet_NaN()));
            }
        }

        dFileDebug(QString("AvgErrorPerImage_%1.txt").arg(imgIdx)) << vector3dToCSV(accuracyImageVector);
    }


    // output global params...
    double avgErrorOfTargets = 0; // distance from true position
    int counter = 0;
    double avgMeanSigmaXOfTargets = 0; // distance from average position
    double avgMeanSigmaYOfTargets = 0; // distance from average position
    double avgMeanSigmaOfTargets = 0;
    double avgTrueSigmaXOfTargets = 0; // distance from true position
    double avgTrueSigmaYOfTargets = 0; // distance from true position
    double avgTrueSigmaOfTargets = 0;
    // double is distance, touple is: number of points, avg error, sigma from mean, sigma from true value


    QVector<cv::Point3d> accuracySceneVector;

    for(int i = 0;
        i < worldPoints_.size();
        ++i)
    {
        if(globTargetAvgErrCollectors_[i]->getDataCount() >= RCE_MIN_OBSERS_GLOBALLY)
        {
            counter++;

            double error = globTargetAvgErrCollectors_[i]->getMean();
            double meanSigmaX = globTargetXCollectors_[i]->getSigma();
            double meanSigmaY = globTargetYCollectors_[i]->getSigma();
            double meanSigma = std::sqrt(meanSigmaX * meanSigmaX + meanSigmaY * meanSigmaY);

            double trueSigmaX = globTargetXCollectors_[i]->getDeviationFromExpectedValue();
            double trueSigmaY = globTargetYCollectors_[i]->getDeviationFromExpectedValue();
            double trueSigma = std::sqrt(trueSigmaX * trueSigmaX + trueSigmaY * trueSigmaY);
            if(std::isnan(error + meanSigmaX + meanSigmaY + meanSigma))
            {
                dDebug() << "Uncertainty Measurement for target" << i << "is NaN:" <<
                            error << meanSigmaX << meanSigmaY << meanSigma;
            }
            avgErrorOfTargets += error;
            avgMeanSigmaXOfTargets += meanSigmaX;
            avgMeanSigmaYOfTargets += meanSigmaY;
            avgMeanSigmaOfTargets += meanSigma;

            avgTrueSigmaXOfTargets += trueSigmaX;
            avgTrueSigmaYOfTargets += trueSigmaY;
            avgTrueSigmaOfTargets += trueSigma;

            accuracySceneVector.push_back(cv::Point3d(worldPoints_[i].x,
                                                      worldPoints_[i].y,
                                                      error));


        }
        else
        {
            accuracySceneVector.push_back(cv::Point3d(worldPoints_[i].x,
                                                      worldPoints_[i].y,
                                                      std::numeric_limits<double>::quiet_NaN()));
        }
    }

    dFileOutput(QString("AvgErrorSpatiallyInScene.txt"))
             << vector3dToCSV(accuracySceneVector);

    dFileDebug("AvgErrorAndSigmaFromImageCentre.txt") << avgMapToStringCSV(avgErrorFromImageCentre);

    dFileDebug("AvgErrorAndSigmaFromLandmarkCentre.txt") << avgMapToStringCSV(avgErrorFromLandmarkCentre);

    dFileDebug("AvgError.txt") << "AvgError" << avgErrorOfTargets / counter <<
                "AvgMeanSigmaX" << avgMeanSigmaXOfTargets / counter <<
                "AvgMeanSigmaY" << avgMeanSigmaYOfTargets / counter <<
                "AvgMeanSigma" << avgMeanSigmaOfTargets / counter <<
                "AvgTrueSigmaX" << avgTrueSigmaXOfTargets / counter <<
                "AvgTrueSigmaY" << avgTrueSigmaYOfTargets / counter <<
                "AvgTrueSigma" << avgTrueSigmaOfTargets / counter;
}

bool
rce::accuracy::RRealExperimentAnalyzer::
allElementsUnique(const std::vector<int> &v)
{
    QSet<int> tmp;
    foreach(int i, v)
    {
        if(tmp.contains(i))
        {
            return false;
        }
        tmp.insert(i);
    }

    return true;
}

void
rce::accuracy::RRealExperimentAnalyzer::
estimateCameraPosition(const std::vector<cv::Point2d> &worldPoints,
                       const std::vector<cv::Point2d> &imagePoints,
                       int i)
{
    if((worldPoints.size() > 3) &&
       (imagePoints.size() > 3))
    {




        cv::Mat homography = cv::findHomography(worldPoints,
                                                imagePoints);


        cv::Mat camMatrix = cv::Mat::eye(3,3,
                                         CV_64FC1);
        camMatrix.at<double>(0,0) = fx_;
        camMatrix.at<double>(1,1) = fy_;
        camMatrix.at<double>(0,2) = principalPoint_.x;
        camMatrix.at<double>(1,2) = principalPoint_.y;

        cv::Point2d sceneCentre = calculateLandmarkCentre(worldPoints);

        std::vector<cv::Point3d> positions;
        std::vector<cv::Mat> rotations;
        std::vector<cv::Point3d> headingVectors;
        std::vector<cv::Point3d> toSceneVectors;
        rce::geometry::RCameraPoseEstimation::estimateCameraPoseRobust(camMatrix,
                                                                 homography,
                                                                 cv::Point3d(sceneCentre.x,
                                                                             sceneCentre.y,
                                                                             0),
                                                                 positions,
                                                                 rotations,
                                                                 headingVectors,
                                                                 toSceneVectors,
                                                                 false);


//        std::vector<cv::Point3d> candPositions;
//        std::vector<cv::Mat> candRotations;
//        rce::geometry::RCameraPoseEstimation::estimatePoseNADIRApproach(camMatrix,
//                                                                        homography,
//                                                                        candPositions,
//                                                                        candRotations,
//                                                                        false);
//        rce::geometry::RCameraPoseEstimation::getReasonableSolutions(candPositions,
//                                                                     candRotations,
//                                                                     cv::Point3d(sceneCentre.x,
//                                                                                 sceneCentre.y,
//                                                                                 0),
//                                                                     positions,
//                                                                     rotations,
//                                                                     headingVectors,
//                                                                     toSceneVectors,
//                                                                     0.4);

        QString message = QObject::tr("Available camera positions:\n");
        for(int j = 0;
            j < positions.size();
            ++j)
        {

            cv::Vec3d eulerAngles = rce::geometry::calculateEulerAnglesFromRotation(rotations[j]);
            message += QObject::tr("\nSolution %1: "
                                   "Position(E=%2 N=%3 H=%4), "
                                   "EulerAngles(%5, %6, %7), "
                                   "HeadingVector(%8 %9 %10),"
                                   "ToSceneVector(%11 %12 %13)")
                       .arg(j)
                       .arg(positions[j].x, 0, 'f', 2)
                       .arg(positions[j].y, 0, 'f', 2)
                       .arg(positions[j].z, 0, 'f', 2)
                       .arg(eulerAngles(0), 0, 'f', 3)
                       .arg(eulerAngles(1), 0, 'f', 3)
                       .arg(eulerAngles(2), 0, 'f', 3)
                       .arg(headingVectors[j].x, 0, 'f', 2)
                       .arg(headingVectors[j].y, 0, 'f', 2)
                       .arg(headingVectors[j].z, 0, 'f', 2)
                       .arg(toSceneVectors[j].x, 0, 'f', 2)
                       .arg(toSceneVectors[j].y, 0, 'f', 2)
                       .arg(toSceneVectors[j].z, 0, 'f', 2);
        }

        dDebug() << message;


        if(RCE_SHOW_VISUALISATION)
        {
            cv::Point2d minPt = worldPoints[0];
            cv::Point2d maxPt = worldPoints[0];
            for(int j = 1;
                j < worldPoints.size();
                ++j)
            {
                if(worldPoints[j].x < minPt.x)
                {
                    minPt.x = worldPoints[j].x;
                }
                else if(worldPoints[j].x > maxPt.x)
                {
                    maxPt.x = worldPoints[j].x;
                }


                if(worldPoints[j].y < minPt.y)
                {
                    minPt.y = worldPoints[j].y;
                }
                else if(worldPoints[j].y > maxPt.y)
                {
                    maxPt.y = worldPoints[j].y;
                }
            }

            cv::Mat shift = cv::Mat::eye(3,3,
                                         CV_64FC1);
            shift.at<double>(0,2) = -minPt.x;
            shift.at<double>(1,2) = -minPt.y;

            homography = shift * (homography.inv());
            cv::Mat vis;
            cv::warpPerspective(images_[i],
                                vis,
                                homography,
                                cv::Size(maxPt.x - minPt.x,
                                         maxPt.y - minPt.y));
            cv::imshow("ViS",
                       vis);
            cv::waitKey();
        }

        if(positions.size() > 0)
        {
            if(i == 0)
            {
                cv::Mat tempRotation = rotations[0].clone();
                //tempRotation.col(2) = (tempRotation.col(2) * -1.0);
                cv::Vec3d eulerAngles = rce::geometry::calculateEulerAnglesFromRotation(tempRotation);
                cv::Mat rotationCheck = rce::geometry::buildRotationMatrixFromEulerAngles(eulerAngles);
                qDebug() << "ROTATION CHECK" << cv::Mat(tempRotation - rotationCheck);
                // save camera position and orientation to config file
                dFileOutput("config_for_simulation.ini") << QString("camera\\position_x=%1\n"
                                                                    "camera\\position_y=%2\n"
                                                                    "camera\\position_z=%3\n"
                                                                    "camera\\rotation_angle_x=%4\n"
                                                                    "camera\\rotation_angle_y=%5\n"
                                                                    "camera\\rotation_angle_z=%6\n")
                                                            .arg(positions[0].x, 0, 'f', 17)
                                                            .arg(positions[0].y, 0, 'f', 17)
                                                            .arg(positions[0].z, 0, 'f', 17)
                                                            .arg(eulerAngles(0), 0, 'f', 17)
                                                            .arg(eulerAngles(1), 0, 'f', 17)
                                                            .arg(eulerAngles(2), 0, 'f', 17);
            }
        }

    }
    else
    {
        qDebug() << "Not enough points to calculate perspective homography for image" << i;
    }
}

void
rce::accuracy::RRealExperimentAnalyzer::
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
rce::accuracy::RRealExperimentAnalyzer::
avgMapToStringCSV(const std::map<double, std::tuple<int, double, double, double> > &avgMap)
{
    QString result;
    for(auto it = avgMap.begin();
        it != avgMap.end();
        ++it)
    {
        const std::tuple<int, double, double,double> &data = it->second;
        for(int i = 0;
            i < std::get<0>(data);
            ++i)
        {
            result += QString("%1; %2; %3; %4;\n")
                  .arg(it->first)
                  .arg(std::get<1>(data) / std::get<0>(data))
                  .arg(std::get<2>(data) / std::get<0>(data))
                  .arg(std::get<3>(data) / std::get<0>(data));
        }

    }
    return result;
}

cv::Point2d
        rce::accuracy::RRealExperimentAnalyzer::
        calculateLandmarkCentre(const std::vector<cv::Point2d> &pts)
{
    cv::Point2d result(0,0);
    for(int i = 0;
        i < pts.size();
        ++i)
    {
        result+=pts[i];
    }
    return result * (1.0 / pts.size());
}

QString
rce::accuracy::RRealExperimentAnalyzer::
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

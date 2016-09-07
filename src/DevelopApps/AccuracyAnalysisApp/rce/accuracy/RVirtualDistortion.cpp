#include "RVirtualDistortion.h"

#include <QSettings>

#include <opencv2/calib3d/calib3d.hpp>


#include "rce/core/RSettingNester.h"
#include "rce/accuracy/ROpenCVDistortion.h"
#include "rce/accuracy/ROpenCVErrDistortion.h"
#include "rce/accuracy/RVirtualCamera.h"


std::shared_ptr<rce::accuracy::RVirtualDistortion>
rce::accuracy::RVirtualDistortion::
createFromSettings(const RVirtualCamera &camera,
                   QSettings &settings,
                   const QString &sectionName)
{
    rce::core::RSettingNester n(settings,
                                sectionName);

    if(!settings.contains("type"))
        settings.setValue("type",
                          "none");

    QString typeStr = settings.value("type").toString().toLower();
    if(typeStr == "opencv")
    {
        if(!settings.contains("k1"))
            settings.setValue("k1", 0);
        double k1 = settings.value("k1").toDouble();
        if(!settings.contains("k2"))
            settings.setValue("k2", 0);
        double k2 = settings.value("k2").toDouble();
        if(!settings.contains("k3"))
            settings.setValue("k3", 0);
        double k3 = settings.value("k3").toDouble();
        if(!settings.contains("k4"))
            settings.setValue("k4", 0);
        double k4 = settings.value("k4").toDouble();
        if(!settings.contains("k5"))
            settings.setValue("k5", 0);
        double k5 = settings.value("k5").toDouble();
        if(!settings.contains("k6"))
            settings.setValue("k6", 0);
        double k6 = settings.value("k6").toDouble();
        if(!settings.contains("p1"))
            settings.setValue("p1", 0);
        double p1 = settings.value("p1").toDouble();
        if(!settings.contains("p2"))
            settings.setValue("p2", 0);
        double p2 = settings.value("p2").toDouble();

        return std::shared_ptr<rce::accuracy::ROpenCVDistortion>(new rce::accuracy::ROpenCVDistortion(camera.getPrincipalPoint(),
                                         camera.getFx(),
                                         camera.getFy(),
                                         k1,k2,k3,k4,k5,k6,p1,p2));
    }
    else if(typeStr == "err_calibration")
    {
        std::shared_ptr<rce::accuracy::RVirtualDistortion> trueDistortion = createFromSettings(camera,
                                                                                              settings,
                                                                                              "true_distortion");
        std::shared_ptr<rce::accuracy::RVirtualDistortion> wrongDistortion = createFromSettings(camera,
                                                                                               settings,
                                                                                               "wrong_distortion");

        std::shared_ptr<rce::accuracy::ROpenCVDistortion> trueDistortionCV = std::dynamic_pointer_cast<rce::accuracy::ROpenCVDistortion>(trueDistortion);
        std::shared_ptr<rce::accuracy::ROpenCVDistortion> wrongDistortionCV = std::dynamic_pointer_cast<rce::accuracy::ROpenCVDistortion>(wrongDistortion);


        if((trueDistortionCV == NULL) ||
           (wrongDistortionCV == NULL))
        {
            throw "You must provide opencv distortion!";
        }

        return std::shared_ptr<rce::accuracy::ROpenCVErrDistortion>(
                    new rce::accuracy::ROpenCVErrDistortion(trueDistortionCV,
                                                            wrongDistortionCV));

    }
    else
    {
        return std::shared_ptr<rce::accuracy::RVirtualDistortion>();
    }
}

void
rce::accuracy::RVirtualDistortion::
distortPointsInImage(const std::vector<cv::Point2d> &pointsInImage,
                     std::vector<cv::Point2d> &distortedPoints) const
{
    distortedPoints.resize(pointsInImage.size());
    for(int i = 0;
        i < pointsInImage.size();
        ++i)
    {
        distortedPoints[i] = distortPointInImage(pointsInImage[i]);
    }
}

void
rce::accuracy::RVirtualDistortion::
distortPointsInCamera3D(const std::vector<cv::Point3d> &pointsIn3D,
                        std::vector<cv::Point2d> &distortedPoints) const
{
    distortedPoints.resize(pointsIn3D.size());
    for(int i = 0;
        i < pointsIn3D.size();
        ++i)
    {
        distortedPoints[i] = distortPointInCamera3D(pointsIn3D[i]);
    }
}

double
rce::accuracy::RVirtualDistortion::
estimateRMSOverImage(const cv::Size &imageSize,
                     int stride) const
{
    double sumSqrErr = 0;
    int count = 0;

    std::vector<cv::Point2d> pointsInImage;
    std::vector<cv::Point2d> distortedPoints;

    pointsInImage.reserve((imageSize.width / stride + 1) *
                          (imageSize.height / stride + 1));
    for(int x = 0;
        x < imageSize.width;
        x += stride)
    {
        for(int y = 0;
            y < imageSize.height;
            y +=  stride)
        {
            pointsInImage.push_back(cv::Point2d(x,y));
        }
    }

    distortPointsInImage(pointsInImage,
                         distortedPoints);

    // calculate projective transformation
    cv::Mat projTransform = cv::findHomography(distortedPoints,
                                               pointsInImage);

    for(int i = 0;
        i < distortedPoints.size();
        ++i)
    {
        cv::Vec3d distPt(distortedPoints[i].x,
                         distortedPoints[i].y,
                         1);
        cv::Vec3d undistPt = cv::Mat(projTransform * cv::Mat(distPt));

        undistPt = undistPt * (1.0 / undistPt(2));


        double sqrErr = cv::norm(cv::Vec2d(pointsInImage[i].x - undistPt(0),
                                           pointsInImage[i].y - undistPt(1)),
                                 cv::NORM_L2SQR);

        sumSqrErr += sqrErr;
        ++count;
    }

    return std::sqrt(sumSqrErr / count);
}

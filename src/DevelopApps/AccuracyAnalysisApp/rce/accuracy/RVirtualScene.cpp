#include "RVirtualScene.h"

#include <QSettings>
#include <QVector3D>
#include <QSize>

#include "rce/core/RSettingNester.h"
#include "rce/accuracy/RVirtualCamera.h"

std::shared_ptr<rce::accuracy::RVirtualScene>
rce::accuracy::RVirtualScene::
createFromSettings(QSettings &settings,
                   const QString &sectionName)
{
    rce::core::RSettingNester n(settings,
                                sectionName);

    std::shared_ptr<RVirtualCamera> camera = RVirtualCamera::createFromSettings(settings,
                                                                                "camera");

    if(!settings.contains("point_of_view_x"))
        settings.setValue("point_of_view_x",
                          0);
    if(!settings.contains("point_of_view_y"))
        settings.setValue("point_of_view_y",
                          0);
    if(!settings.contains("point_of_view_z"))
        settings.setValue("point_of_view_z",
                          0);
    cv::Point3d pointOfView(settings.value("point_of_view_x").toDouble(),
                            settings.value("point_of_view_y").toDouble(),
                            settings.value("point_of_view_z").toDouble());

    // rotate camera to point of view
    if(!settings.contains("rotate_to_point_of_view"))
        settings.setValue("rotate_to_point_of_view",
                          true);
    if(settings.value("rotate_to_point_of_view").toBool())
    {
        camera->rotateTo(pointOfView);
    }

    if(!settings.contains("landmarks/size"))
    {
        settings.beginWriteArray("landmarks",
                                 4);
        settings.setArrayIndex(0);
        settings.setValue("landmark_x",
                          0);
        settings.setValue("landmark_y",
                          0);
        settings.setArrayIndex(1);
        settings.setValue("landmark_x",
                          0);
        settings.setValue("landmark_y",
                          1);
        settings.setArrayIndex(2);
        settings.setValue("landmark_x",
                          1);
        settings.setValue("landmark_y",
                          0);
        settings.setArrayIndex(3);
        settings.setValue("landmark_x",
                          1);
        settings.setValue("landmark_y",
                          1);
        settings.endArray();
    }
    std::vector<cv::Point2d> landmarks;

    int landmarksCount = settings.beginReadArray("landmarks");
    landmarks.reserve(landmarksCount);
    for(int i = 0;
        i < landmarksCount;
        ++i)
    {
        settings.setArrayIndex(i);
        landmarks.push_back(cv::Point2d(settings.value("landmark_x").toDouble(),
                                        settings.value("landmark_y").toDouble()));
    }
    settings.endArray();



    if(!settings.contains("targets_definition"))
        settings.setValue("targets_definition",
                          "grid");
    std::vector<cv::Point2d> targets;

    if(settings.value("targets_definition").toString().toLower() == "grid")
    {
        settings.beginGroup("targets_grid");
        {
            if(!settings.contains("origin_x"))
                settings.setValue("origin_x",
                                  -5);
            if(!settings.contains("origin_y"))
                settings.setValue("origin_y",
                                  -5);
            QPointF origin(settings.value("origin_x").toDouble(),
                           settings.value("origin_y").toDouble());

            if(!settings.contains("stride_x"))
                settings.setValue("stride_x",
                                  1);
            if(!settings.contains("stride_y"))
                settings.setValue("stride_y",
                                  1);
            QPointF stride(settings.value("stride_x").toDouble(),
                           settings.value("stride_y").toDouble());

            if(!settings.contains("size"))
                settings.setValue("size",
                                  QSize(10,10));
            QSize size = settings.value("size").toSize();

            targets.reserve(size.width() * size.height());

            for(int i = 0;
                i < size.height();
                ++i)
            {
                for(int j = 0;
                    j < size.width();
                    ++j)
                {
                    targets.push_back(cv::Point2d(origin.x() + stride.x() * j,
                                                  origin.y() + stride.y() * i));
                }
            }
        }
        settings.endGroup();
    }
    else if(settings.value("targets_definition").toString().toLower() == "list")
    {
        if(!settings.contains("targets_list/size"))
        {
            settings.beginWriteArray("targets_list",
                                     1);
            settings.setArrayIndex(0);
            settings.setValue("target_x",0);
            settings.setValue("target_y",0);
            settings.endArray();
        }

        int targetsCount = settings.beginReadArray("targets_list");
        targets.reserve(targetsCount);
        for(int i = 0;
            i < targetsCount;
            ++i)
        {
            settings.setArrayIndex(i);
            QPointF targetPt(settings.value("target_x").toDouble(),
                             settings.value("target_y").toDouble());
            targets.push_back(cv::Point2d(targetPt.x(),
                                          targetPt.y()));
        }
        settings.endArray();
    }

    std::vector<double> landamarkElevationErrors;
    landamarkElevationErrors.resize(landmarks.size(), 0);
    std::vector<double> targetElevationErrors;
    targetElevationErrors.resize(targets.size(), 0);


    std::shared_ptr<RVirtualScene> result(new RVirtualScene(camera,
                                                            pointOfView,
                                                            landmarks,
                                                            targets,
                                                            landamarkElevationErrors,
                                                            targetElevationErrors));

    return result;

}

rce::accuracy::RVirtualScene::
RVirtualScene(std::shared_ptr<rce::accuracy::RVirtualCamera> camera,
              const cv::Point3d &pointOfView,
              const std::vector<cv::Point2d> &landMarks,
              const std::vector<cv::Point2d> &targets,
              const std::vector<double> &landMarkElevationErrors,
              const std::vector<double> &targetElevationErrors):
    camera_(camera),
    pointOfView_(pointOfView),
    landmarks_(landMarks),
    landMarkElevationErrors_(landMarkElevationErrors),
    targets_(targets),
    targetElevationErrors_(targetElevationErrors)
{

}

void
rce::accuracy::RVirtualScene::
captureLandmarks(std::vector<cv::Point2d> &imagedLandmarks,
                 std::vector<uchar> &insideImageFlags,
                 bool enableDistortion)
{
    capturePointsOld(landmarks_,
                     landMarkElevationErrors_,
                  imagedLandmarks,
                  insideImageFlags,
                  enableDistortion);
}

void
rce::accuracy::RVirtualScene::
captureTargets(std::vector<cv::Point2d> &imagedTargets,
               std::vector<uchar> &insideImageFlags,
               bool enableDistortion,
               const std::vector<uchar> &captureFlags)
{
    capturePointsOld(targets_,
                     targetElevationErrors_,
                     imagedTargets,
                     insideImageFlags,
                     enableDistortion,
                     captureFlags);
}

cv::Point2d
rce::accuracy::RVirtualScene::
calculateLandmarkCentre() const
{
    cv::Point2d result(0,0);
    for(int i = 0;
        i < getLandmarksCount();
        ++i)
    {
        result += landmarks_[i];
    }

    return result * (1.0 / getLandmarksCount());
}

void
rce::accuracy::RVirtualScene::
capturePointsOld(const std::vector<cv::Point2d> &points,
                 const std::vector<double> &elevationNoise,
              std::vector<cv::Point2d> &imagedPoints,
              std::vector<uchar> &validFlags,
              bool enableDistortion,
                 std::vector<uchar> captureFlags)
{
    if(captureFlags.size() != points.size())
    {
        captureFlags.resize(points.size(), 255);
    }

    std::shared_ptr<RVirtualDistortion> formerDistortion;
    if(!enableDistortion)
    {
        formerDistortion = camera_->getDistortion();
        camera_->setDistortion(NULL);
    }
    imagedPoints.resize(points.size());
    validFlags.resize(points.size());

    for(size_t i = 0;
        i < points.size();
        ++i)
    {
        if(captureFlags[i])
        {
            bool insideImageFlag, inFrontOfCameraFlag;
            cv::Point2d projection = camera_->projectPoint(cv::Point3d(points[i].x,
                                                                       points[i].y,
                                                                       elevationNoise[i]),
                                                           &insideImageFlag,
                                                           &inFrontOfCameraFlag);
            imagedPoints[i] = projection;
            validFlags[i] = insideImageFlag && inFrontOfCameraFlag ? 255 : 0;
        }
        else
        {
            validFlags[i] = 0;
        }
    }
    if(!enableDistortion)
    {
        camera_->setDistortion(formerDistortion);
    }
}

void
rce::accuracy::RVirtualScene::
capturePointsNew(const std::vector<cv::Point2d> &points,
                 std::vector<cv::Point2d> &imagedPoints,
                 std::vector<uchar> &validFlags,
                 bool enableDistortion,
                 const std::vector<uchar> &captureFlags)
{


    std::shared_ptr<RVirtualDistortion> formerDistortion;
    if(!enableDistortion)
    {
        formerDistortion = camera_->getDistortion();
        camera_->setDistortion(NULL);
    }

    if(captureFlags.empty())
    {
        imagedPoints.resize(points.size());
        validFlags.resize(points.size());
        std::vector<uchar> inisideImageFlags(points.size());
        std::vector<uchar> inFrontFlags(points.size());

        std::vector<cv::Point3d> worldPoints(points.size());
        for(int i = 0;
            i < worldPoints.size();
            ++i)
        {
            worldPoints[i].x = points[i].x;
            worldPoints[i].y = points[i].y;
            worldPoints[i].z = 0;
        }

        camera_->projectPoints(worldPoints,
                               imagedPoints,
                               inisideImageFlags,
                               inFrontFlags);
        for(int i = 0;
            i < validFlags.size();
            ++i)
        {
            validFlags[i] = (inisideImageFlags[i] != 0) && (inFrontFlags[i] != 0) ? 255 : 0;
        }
    }
    else
    {

        // prepare data
        std::vector<int> selectedIndices;
        std::vector<cv::Point2d> selectedPoints;
        std::vector<cv::Point2d> selectedImagedPoints;
        std::vector<uchar> selectedValidFlags;
        selectedPoints.reserve(points.size());
        selectedImagedPoints.reserve(points.size());
        selectedValidFlags.reserve(points.size());
        selectedIndices.reserve(points.size());
        for(int i = 0;
            i < captureFlags.size();
            ++i)
        {
            if(captureFlags[i] != 0)
            {
                selectedPoints.push_back(points[i]);
                selectedIndices.push_back(i);
            }
        }

        // capture them
        capturePointsNew(selectedPoints,
                         selectedImagedPoints,
                         selectedValidFlags,
                         enableDistortion);

        // get data back
        validFlags.clear();
        validFlags.resize(points.size(),
                          0);
        imagedPoints.resize(points.size());
        for(int i = 0;
            i < selectedPoints.size();
            ++i)
        {
            int idx = selectedIndices[i];
            imagedPoints[idx] = selectedPoints[i];
            validFlags[idx] = selectedValidFlags[i];
        }
    }



    if(!enableDistortion)
    {
        camera_->setDistortion(formerDistortion);
    }
}

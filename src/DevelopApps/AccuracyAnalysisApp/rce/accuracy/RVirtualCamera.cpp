#include "RVirtualCamera.h"

#include <iostream>

#include <QSettings>
#include <QSize>
#include <QVector3D>

#include <opencv2/calib3d/calib3d.hpp>

#include "rce/geometry/RGeometry.h"
#include "rce/core/RSettingNester.h"
#include "dfs/core/DDebug.h"
#include "rce/utility/ROpenCVtoQDebug.h"
#include "rce/accuracy/RVirtualDistortion.h"

std::shared_ptr<rce::accuracy::RVirtualCamera>
rce::accuracy::RVirtualCamera::
createFromSettings(QSettings &settings,
                   const QString &sectionName)
{
    rce::core::RSettingNester n(settings,
                                sectionName);


    std::shared_ptr<RVirtualCamera> result(new RVirtualCamera(0,0,0,0,
                                                              cv::Size(),
                                                              cv::Point3d(),
                                                              cv::Point3d()));

    if(!settings.contains("fx"))
        settings.setValue("fx",
                          1);
    result->fx_ = settings.value("fx").toDouble();

    if(!settings.contains("fy"))
        settings.setValue("fy",
                          1);
    result->fy_ = settings.value("fy").toDouble();

    if(!settings.contains("cx"))
        settings.setValue("cx",
                          1);
    result->cx_ = settings.value("cx").toDouble();

    if(!settings.contains("cy"))
        settings.setValue("cy",
                          1);
    result->cy_ = settings.value("cy").toDouble();

    if(!settings.contains("image_resolution"))
        settings.setValue("image_resolution",
                          QSize(2,2));
    QSize imageResiltionQ = settings.value("image_resolution").toSize();
    result->imageResolution_.width = imageResiltionQ.width();
    result->imageResolution_.height = imageResiltionQ.height();

    if(!settings.contains("position_x"))
        settings.setValue("position_x",
                          0);
    if(!settings.contains("position_y"))
        settings.setValue("position_y",
                          0);
    if(!settings.contains("position_z"))
        settings.setValue("position_z",
                          5);
    result->position_.x = settings.value("position_x").toDouble();
    result->position_.y = settings.value("position_y").toDouble();
    result->position_.z = settings.value("position_z").toDouble();

    if(!settings.contains("rotation_angle_x"))
        settings.setValue("rotation_angle_x",
                          M_PI);
    if(!settings.contains("rotation_angle_y"))
        settings.setValue("rotation_angle_y",
                          0);
    if(!settings.contains("rotation_angle_z"))
        settings.setValue("rotation_angle_z",
                          0);
    result->rotationAngles_.x = settings.value("rotation_angle_x").toDouble();
    result->rotationAngles_.y = settings.value("rotation_angle_y").toDouble();
    result->rotationAngles_.z = settings.value("rotation_angle_z").toDouble();

    result->distortion_ = rce::accuracy::RVirtualDistortion::createFromSettings(*result,
                                                                                settings,
                                                                                "distortion");
    if(result->distortion_ != NULL)
    {
        dDebug() << "Camera distortion RMS error:" << result->distortion_->estimateRMSOverImage(result->imageResolution_,
                                          10);
    }

    return result;

}

rce::accuracy::RVirtualCamera::
RVirtualCamera(double fx,
               double fy,
               double cx,
               double cy,
               const cv::Size &imageResolution,
               const cv::Point3d &position,
               const cv::Point3d &rotationAngles):
    fx_(fx),
    fy_(fy),
    cx_(cx),
    cy_(cy),
    imageResolution_(imageResolution),
    position_(position),
    rotationAngles_(rotationAngles)
{

}

std::shared_ptr<rce::accuracy::RVirtualCamera>
rce::accuracy::RVirtualCamera::
cloneShared() const
{
    std::shared_ptr<RVirtualCamera> result(new RVirtualCamera(*this));
    result->cameraMatrix_ = result->cameraMatrix_.clone();
    result->transformationMatrix_ = result->transformationMatrix_.clone();
    result->projectionMatrix_ = result->projectionMatrix_.clone();
    return result;
}

cv::Point2d
rce::accuracy::RVirtualCamera::
getPrincipalPoint() const
{
    return cv::Point2d(cx_,
                       cy_);
}

void
rce::accuracy::RVirtualCamera::
projectPoints(const std::vector<cv::Point3d> &worldPoints,
              std::vector<cv::Point2d> &imagePoints,
              std::vector<uchar> &insideImageFlags,
              std::vector<uchar> &inFrontCameraFlags)
{
    // prepare world points vector of vectors
    insideImageFlags.resize(worldPoints.size());
    inFrontCameraFlags.resize(worldPoints.size());
    imagePoints.resize(worldPoints.size());
    cv::Mat worldPointsVecs(4, worldPoints.size(),
                            CV_64FC1);
    for(int i = 0;
        i < worldPoints.size();
        ++i)
    {
        worldPointsVecs.at<double>(0,i) = worldPoints[i].x;
        worldPointsVecs.at<double>(1,i) = worldPoints[i].y;
        worldPointsVecs.at<double>(2,i) = worldPoints[i].z;
        worldPointsVecs.at<double>(3,i) = 1;
    }


    prepareTransformationMatrix();
    prepareCameraMatrix();


    cv::Mat tranformedPoints = transformationMatrix_ * worldPointsVecs;


    if(distortion_)
    {

        std::vector<cv::Point3d> camera3DPoints(worldPoints.size());
        for(int i = 0;
            i < worldPoints.size();
            ++i)
        {
            double w = tranformedPoints.at<double>(3,i);
            camera3DPoints[i].x = tranformedPoints.at<double>(0,i) / w;
            camera3DPoints[i].y = tranformedPoints.at<double>(1,i) / w;
            camera3DPoints[i].z = tranformedPoints.at<double>(2,i) / w;
            inFrontCameraFlags[i] = (camera3DPoints[i].z > 0) ? 255:0;
        }

        distortion_->distortPointsInCamera3D(camera3DPoints,
                                             imagePoints);

    }
    else
    {

        imagePoints.resize(worldPoints.size());
        for(int i = 0;
            i < worldPoints.size();
            ++i)
        {
            double z = tranformedPoints.at<double>(2,i);
            double w = tranformedPoints.at<double>(3,i);
            imagePoints[i].x = tranformedPoints.at<double>(0,i) / z;
            imagePoints[i].y = tranformedPoints.at<double>(1,i) / z;
            // we can disregard fourth (homogenous coordinate), as (x/w) / (z/w) == (x/z)
            inFrontCameraFlags[i] = ((z / w) > 0) ? 255:0;
        }
    }

    // now just project them into pixel space from projection plane
    for(int i = 0;
        i < imagePoints.size();
        ++i)
    {
        // we do not need to use camera matrix as this gives less computations
        imagePoints[i].x = imagePoints[i].x * fx_ + cx_;
        imagePoints[i].y = imagePoints[i].y * fy_ + cy_;

        insideImageFlags[i] = rce::geometry::isPointInsideSize(imagePoints[i],
                                                               imageResolution_) ?
                                  255:0;
    }
}

cv::Point2d
rce::accuracy::RVirtualCamera::
projectPoint(const cv::Point3d &worldPoint,
             bool *insideImage,
             bool *inFrontOfCamera)
{
    cv::Point2d imagePt;

    cv::Vec4d worldVec(worldPoint.x,
                       worldPoint.y,
                       worldPoint.z,
                       1);

    if(inFrontOfCamera != NULL)
    {
        // two steps projection (to check whether it is in front of camera)
        prepareTransformationMatrix();
        prepareCameraMatrix();

        cv::Vec4d camera3DVec = cv::Mat(transformationMatrix_ * cv::Mat(worldVec));
        camera3DVec = camera3DVec * (1.0 / camera3DVec(3));
        *inFrontOfCamera = (camera3DVec(3) > 0);

        cv::Vec3d imageVec = cv::Mat(cameraMatrix_ * cv::Mat(camera3DVec));
        imagePt.x = imageVec(0) / imageVec(2);
        imagePt.y = imageVec(1) / imageVec(2);
    }
    else
    {
        // direct projection through projectionMatrix_
        prepareProjectionMatrix();

        cv::Vec3d imageVec = cv::Mat(projectionMatrix_ * cv::Mat(worldVec));
        imagePt.x = imageVec(0) / imageVec(2);
        imagePt.y = imageVec(1) / imageVec(2);
    }

    if(insideImage != NULL)
    {
        // check if it is inside image
        *insideImage = rce::geometry::isPointInsideSize(imagePt,
                                                       imageResolution_);
    }

    dInfo() << "RVirtualCamera::projectPoint" <<
               worldPoint.x << worldPoint.y << worldPoint.z << "to" <<
               imagePt.x << imagePt.y;


    if(distortion_)
    {
        imagePt = distortion_->distortPointInImage(imagePt);
        dInfo() << "RVirtualCamera::projectPoint: distorted:" << imagePt.x << imagePt.y;
    }


    return imagePt;
}

void
rce::accuracy::RVirtualCamera::
rotateTo(const cv::Point3d &pointOfView)
{
    resetTransformationMatrix();

    cv::Point3d direction = pointOfView - position_;

    double directionSize = cv::norm(direction);
    if(directionSize == 0)
    {
        // just reset the rotation
        // with 0,0,0, camera is facing straight up in direction of z axis
        rotationAngles_.x = 0;
        rotationAngles_.y = 0;
        rotationAngles_.z = 0;
    }
    else
    {
        cv::Vec3d directionVec(direction.x / directionSize,
                               direction.y / directionSize,
                               direction.z / directionSize);
        cv::Vec3d camDefDirection(0,0,1);

        cv::Mat rotation = rce::geometry::alignUnitVectorsByRotation(camDefDirection,
                                                                     directionVec);

        cv::Vec3d rotationsVec = rce::geometry::calculateEulerAnglesFromRotation(rotation);
        rotationAngles_.x = rotationsVec(0);
        rotationAngles_.y = rotationsVec(1);
        rotationAngles_.z = rotationsVec(2);

        // FIXME: Remove this, just for debug:
//        {
//            prepareTransformationMatrix();

//            cv::Vec4d pointOfViewVector(pointOfView.x,
//                                            pointOfView.y,
//                                            pointOfView.z,
//                                            1);

//            pointOfViewVector = cv::Mat(transformationMatrix_ * cv::Mat(pointOfViewVector));

//            dInfo() << "RVirtualCamera::rotateTo:" << direction.x << direction.y << direction.z <<
//                       "mapped to" <<
//                       pointOfViewVector(0) / pointOfViewVector(3) <<
//                       pointOfViewVector(1) / pointOfViewVector(3) <<
//                       pointOfViewVector(2) / pointOfViewVector(3);
//        }

    }

}

void
rce::accuracy::RVirtualCamera::
prepareTransformationMatrix()
{
    if(transformationMatrix_.empty())
    {
        transformationMatrix_ = cv::Mat(4,4,
                                        CV_64FC1);
        cv::Mat translationMatrix = cv::Mat::eye(4,4,
                                            CV_64FC1);
        translationMatrix.at<double>(0,3) = -position_.x;
        translationMatrix.at<double>(1,3) = -position_.y;
        translationMatrix.at<double>(2,3) = -position_.z;

        cv::Mat rotationMatrix = cv::Mat::eye(4,4,
                                              CV_64FC1);
        cv::Mat rotation = rce::geometry::buildRotationMatrixFromEulerAngles(cv::Vec3d(rotationAngles_.x,
                                                                                       rotationAngles_.y,
                                                                                       rotationAngles_.z));
//        cv::Rodrigues(cv::Mat(rotationAngles_), // THIS IS WRONG!
//                      rotation);

        rotation = rotation.t(); // "The inverse of a rotation matrix is its transpose"
        rotation.copyTo(rotationMatrix.colRange(0,3).rowRange(0,3));


        transformationMatrix_ = rotationMatrix * translationMatrix;

        dInfo() << "prepareTransformationMatrix" <<
                     transformationMatrix_ << "\n" <<
                     rotationMatrix << "\n" <<
                     translationMatrix << "\n";
    }
}

void
rce::accuracy::RVirtualCamera::
prepareCameraMatrix()
{

    if(cameraMatrix_.empty())
    {
        // just a simple camera matrix with appended last zero column
        cameraMatrix_ = cv::Mat::zeros(3,4,
                                CV_64FC1);
        cameraMatrix_.at<double>(0,0) = fx_;
        cameraMatrix_.at<double>(1,1) = fy_;
        cameraMatrix_.at<double>(2,2) = 1.0;
        cameraMatrix_.at<double>(0,2) = cx_;
        cameraMatrix_.at<double>(1,2) = cy_;

        dInfo() << "prepareCameraMatrix" << cameraMatrix_ << "\n" << "\n";
    }
}

void
rce::accuracy::RVirtualCamera::
prepareProjectionMatrix()
{

    if(projectionMatrix_.empty())
    {
        prepareTransformationMatrix();
        prepareCameraMatrix();

        projectionMatrix_ = cameraMatrix_ * transformationMatrix_;
    }
}

void
rce::accuracy::RVirtualCamera::
resetTransformationMatrix()
{
    transformationMatrix_.release();
    projectionMatrix_.release();
}

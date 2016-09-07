#include "ROpenCVErrDistortion.h"

#include "rce/accuracy/ROpenCVDistortion.h"

rce::accuracy::ROpenCVErrDistortion::
ROpenCVErrDistortion(std::shared_ptr<ROpenCVDistortion> trueDistortion,
                     std::shared_ptr<ROpenCVDistortion> wrongDistortion):
    trueDistortion_(trueDistortion),
    wrongDistortion_(wrongDistortion)
{
    assert(trueDistortion_ != NULL);
    assert(wrongDistortion_ != NULL);
}

cv::Point2d
rce::accuracy::ROpenCVErrDistortion::
distortPointInImage(const cv::Point2d &pt) const
{

    std::vector<cv::Point2d> camDistortedPoints;
    camDistortedPoints.push_back(trueDistortion_->distortPointInImage(pt));
    std::vector<cv::Point2d> undistortedPoints;

    wrongDistortion_->undistortPointsInImage(camDistortedPoints,
                                             undistortedPoints);
    return undistortedPoints[0];

}

void
rce::accuracy::ROpenCVErrDistortion::
distortPointsInImage(const std::vector<cv::Point2d> &pointsInImage,
                     std::vector<cv::Point2d> &distortedPoints) const
{
    std::vector<cv::Point2d> camDistortedPoints;
    trueDistortion_->distortPointsInImage(pointsInImage,
                                          camDistortedPoints);

    wrongDistortion_->undistortPointsInImage(camDistortedPoints,
                                             distortedPoints);
}

cv::Point2d
rce::accuracy::ROpenCVErrDistortion::
distortPointInCamera3D(const cv::Point3d &pt) const
{
    std::vector<cv::Point2d> camDistortedPoints;
    camDistortedPoints.push_back(trueDistortion_->distortPointInCamera3D(pt));
    std::vector<cv::Point2d> undistortedPoints;

    wrongDistortion_->undistortPointsInProjectionPlane(camDistortedPoints,
                                             undistortedPoints);
    return undistortedPoints[0];
}

void
rce::accuracy::ROpenCVErrDistortion::
distortPointsInCamera3D(const std::vector<cv::Point3d> &pointsIn3D,
                        std::vector<cv::Point2d> &distortedPoints) const
{
    std::vector<cv::Point2d> camDistortedPoints;
    trueDistortion_->distortPointsInCamera3D(pointsIn3D,
                                          camDistortedPoints);
    wrongDistortion_->undistortPointsInProjectionPlane(camDistortedPoints,
                                                       distortedPoints);
}

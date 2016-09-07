#include "ROpenCVDistortion.h"


#include <opencv2/imgproc/imgproc.hpp>

rce::accuracy::ROpenCVDistortion::
ROpenCVDistortion(const cv::Point2d &principalPoint,
                  double fx,
                  double fy,
                  double k1,
                  double k2,
                  double k3,
                  double k4,
                  double k5,
                  double k6,
                  double p1,
                  double p2):
    principalPoint_(principalPoint),
    fx_(fx),
    fy_(fy),
    k1_(k1),
    k2_(k2),
    k3_(k3),
    k4_(k4),
    k5_(k5),
    k6_(k6),
    p1_(p1),
    p2_(p2)
{

}

cv::Point2d
rce::accuracy::ROpenCVDistortion::
distortPointInImage(const cv::Point2d &pt) const
{
    // expect, that pt is perfectly projected without any distortion
    double xDash = (pt.x - principalPoint_.x) / fx_;
    double yDash = (pt.y - principalPoint_.y) / fy_;
    cv::Point2d distortedPoint = distortPointInProjectionPlane(cv::Point2d(xDash,
                                                                           yDash));
    return cv::Point2d(fx_ * distortedPoint.x + principalPoint_.x,
                       fy_ * distortedPoint.y + principalPoint_.y);

}

cv::Point2d
rce::accuracy::ROpenCVDistortion::
distortPointInProjectionPlane(const cv::Point2d &pt) const
{
    double xDash = pt.x;
    double yDash = pt.y;
    double r2 = xDash * xDash + yDash * yDash;

    double k = (1+k1_*r2 + k2_*r2*r2 + k3_*r2*r2*r2)/
               (1+k4_*r2 + k5_*r2*r2 + k6_*r2*r2*r2);
    double xDD = xDash * k +
                 2*p1_*xDash*yDash +
                 p2_*(r2+2*xDash*xDash);
    double yDD = yDash *k +
                 p1_*(r2+2*yDash*yDash) +
                 2*p2_*(xDash*yDash);

    return cv::Point2d(xDD,
                       yDD);
}

cv::Point2d
rce::accuracy::ROpenCVDistortion::
distortPointInCamera3D(const cv::Point3d &pt) const
{
    double xDash = pt.x / pt.z;
    double yDash = pt.y / pt.z;

    return distortPointInProjectionPlane(cv::Point2d(xDash,
                                                     yDash));

}

void
rce::accuracy::ROpenCVDistortion::
undistortPointsInImage(const std::vector<cv::Point2d> &distortedPoints,
                       std::vector<cv::Point2d> &undistortedPoints) const
{
    cv::Mat cameraMatrix = cv::Mat::eye(3,3, CV_64FC1);
    cameraMatrix.at<double>(0,0) = fx_;
    cameraMatrix.at<double>(1,1) = fy_;
    cameraMatrix.at<double>(0,2) = principalPoint_.x;
    cameraMatrix.at<double>(1,2) = principalPoint_.x;

    std::vector<double> distCoeffVector;
    distCoeffVector.push_back(k1_);
    distCoeffVector.push_back(k2_);
    distCoeffVector.push_back(p1_);
    distCoeffVector.push_back(p2_);
    distCoeffVector.push_back(k3_);
    distCoeffVector.push_back(k4_);
    distCoeffVector.push_back(k5_);
    distCoeffVector.push_back(k6_);
    cv::undistortPoints(distortedPoints,
                        undistortedPoints,
                        cameraMatrix,
                        distCoeffVector);
}

void
rce::accuracy::ROpenCVDistortion::
undistortPointsInProjectionPlane(const std::vector<cv::Point2d> &distortedPoints,
                                 std::vector<cv::Point2d> &undistortedPoints) const
{
    cv::Mat cameraMatrix = cv::Mat::eye(3,3, CV_64FC1);

    std::vector<double> distCoeffVector;
    distCoeffVector.push_back(k1_);
    distCoeffVector.push_back(k2_);
    distCoeffVector.push_back(p1_);
    distCoeffVector.push_back(p2_);
    distCoeffVector.push_back(k3_);
    distCoeffVector.push_back(k4_);
    distCoeffVector.push_back(k5_);
    distCoeffVector.push_back(k6_);
    cv::undistortPoints(distortedPoints,
                        undistortedPoints,
                        cameraMatrix,
                        distCoeffVector);
}

#ifndef RCE_ACCURACY_RVIRTUALCAMERA_H
#define RCE_ACCURACY_RVIRTUALCAMERA_H

#include <memory>

#include <QString>

#include <opencv2/core/core.hpp>

class QSettings;

namespace rce {
    namespace accuracy {

        class RVirtualDistortion;

        class RVirtualCamera
        {
        public:



            static std::shared_ptr<RVirtualCamera> createFromSettings(QSettings &settings,
                                                                      const QString &sectionName = QString());

            RVirtualCamera(double fx, // intrinsics
                           double fy,
                           double cx,
                           double cy,
                           const cv::Size &imageResolution,
                           const cv::Point3d &position, // extrinsics
                           const cv::Point3d &rotationAngles);

            std::shared_ptr<RVirtualCamera> cloneShared() const;
            cv::Point2d getPrincipalPoint() const;
            double getFx() const {return fx_;}
            double getFy() const {return fy_;}


            void setDistortion(std::shared_ptr<RVirtualDistortion> distortion) { distortion_ = distortion;}

            std::shared_ptr<RVirtualDistortion> getDistortion() const {return distortion_;}




            void projectPoints(const std::vector<cv::Point3d> &worldPoints,
                               std::vector<cv::Point2d> &imagePoints,
                               std::vector<uchar> &insideImageFlags,
                               std::vector<uchar> &inFrontCameraFlags);

            cv::Point2d projectPoint(const cv::Point3d &worldPoint,
                                     bool *insideImage = NULL,
                                     bool *inFrontOfCamera = NULL);

            void rotateTo(const cv::Point3d& pointOfView);

            void prepareTransformationMatrix();
            void prepareCameraMatrix();
            void prepareProjectionMatrix();

            cv::Size getResolution() const {return imageResolution_;}


            void resetCameraMatrix(); // should be called when intrinsic changes (affecting cameraMatrix)
            void resetTransformationMatrix(); // should be called when extrinsic changes (affecting transformation matrix)
        protected:
            // input parameters
            double fx_;
            double fy_;
            double cx_;
            double cy_;
            cv::Size imageResolution_;

            cv::Point3d position_;

            /**
             * rotationAngles_.x = rotation angle around axis x (roll), from y axis
             * rotationAngles_.y = rotation angle around axis y (pitch), from z axis
             * rotationAngles_.z = rotation angle around axis z (yaw), from x axis
             * The rotation is applied in order x,y,z (roll, pitch, yaw)
             */
            cv::Point3d rotationAngles_;

            // distortion
            std::shared_ptr<RVirtualDistortion> distortion_;

            // precalculated data
            cv::Mat cameraMatrix_; // (3 * 4) projects stuff from camera 3D coordinate system onto image plane
            cv::Mat transformationMatrix_; // transforms world into camera 3D coordinate system

            cv::Mat projectionMatrix_;


        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_RVIRTUALCAMERA_H

#ifndef RCE_ACCURACY_RVIRTUALDISTORTION_H
#define RCE_ACCURACY_RVIRTUALDISTORTION_H

#include <memory>

#include <QString>

#include <opencv2/core/core.hpp>

class QSettings;

namespace rce {
    namespace accuracy {
        class RVirtualCamera;

        class RVirtualDistortion
        {
        public:

            static std::shared_ptr<RVirtualDistortion> createFromSettings(const RVirtualCamera &camera,
                                                                          QSettings &settings,
                                                                          const QString &sectionName = QString());

            RVirtualDistortion(){}



            // pt is in image 2D coordinate system (in pixels)
            // the resulting point is in image coordinates
            virtual cv::Point2d distortPointInImage(const cv::Point2d &pt) const = 0;
            virtual void distortPointsInImage(const std::vector<cv::Point2d> &pointsInImage,
                                      std::vector<cv::Point2d> &distortedPoints) const;


            // pt is in camera 3D coordinate system (in front of camera)
            // the resulting point is in projection plane coordinates (and is yet to be multiplied by camera matrix to yield pixel coordinates)
            virtual cv::Point2d distortPointInCamera3D(const cv::Point3d &pt) const = 0;
            virtual void distortPointsInCamera3D(const std::vector<cv::Point3d> &pointsIn3D,
                                      std::vector<cv::Point2d> &distortedPoints) const;


            virtual double estimateRMSOverImage(const cv::Size &imageSize,
                                                int stride = 1) const;


        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_RVIRTUALDISTORTION_H

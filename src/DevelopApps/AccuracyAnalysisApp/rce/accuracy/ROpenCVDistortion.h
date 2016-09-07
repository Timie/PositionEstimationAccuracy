#ifndef RCE_ACCURACY_ROPENCVDISTORTION_H
#define RCE_ACCURACY_ROPENCVDISTORTION_H

#include "rce/accuracy/RVirtualDistortion.h"

namespace rce {
    namespace accuracy {

        class ROpenCVDistortion : public rce::accuracy::RVirtualDistortion
        {
        public:
            ROpenCVDistortion(const cv::Point2d &principalPoint,
                              double fx,
                              double fy,
                              double k1,
                              double k2 = 0,
                              double k3 = 0,
                              double k4 = 0,
                              double k5 = 0,
                              double k6 = 0,
                              double p1 = 0,
                              double p2 = 0);


            virtual cv::Point2d distortPointInImage(const cv::Point2d &pt) const;
            virtual cv::Point2d distortPointInProjectionPlane(const cv::Point2d &pt) const;



            // when using this, principal point, fx and fy does not need to be set.
            virtual cv::Point2d distortPointInCamera3D(const cv::Point3d &pt) const;


            void undistortPointsInImage(const std::vector<cv::Point2d> &distortedPoints,
                                        std::vector<cv::Point2d> &undistortedPoints) const;

            void undistortPointsInProjectionPlane(const std::vector<cv::Point2d> &distortedPoints,
                                        std::vector<cv::Point2d> &undistortedPoints) const;

        protected:
            cv::Point2d principalPoint_; // in pixel coordinates
            double fx_;
            double fy_;

            double k1_;
            double k2_;
            double k3_;
            double k4_;
            double k5_;
            double k6_;
            double p1_;
            double p2_;
        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_ROPENCVDISTORTION_H

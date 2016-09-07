#ifndef RCE_ACCURACY_ROPENCVERRDISTORTION_H
#define RCE_ACCURACY_ROPENCVERRDISTORTION_H

#include <memory>

#include "rce/accuracy/RVirtualDistortion.h"


namespace rce {
    namespace accuracy {

        class ROpenCVDistortion;


        /**
         * @brief The ROpenCVErrDistortion class simulates inaccuracy of camera calibration.
         * It simulate situation when the camera has true distortion parameters dist_true,
         * but it is used with distortion params dist_wrong
         */
        class ROpenCVErrDistortion : public rce::accuracy::RVirtualDistortion
        {
        public:

            ROpenCVErrDistortion(std::shared_ptr<ROpenCVDistortion> trueDistortion,
                                 std::shared_ptr<ROpenCVDistortion> wrongDistortion);


            virtual cv::Point2d distortPointInImage(const cv::Point2d &pt) const;
            virtual void distortPointsInImage(const std::vector<cv::Point2d> &pointsInImage,
                                      std::vector<cv::Point2d> &distortedPoints) const;



            virtual cv::Point2d distortPointInCamera3D(const cv::Point3d &pt) const;
            virtual void distortPointsInCamera3D(const std::vector<cv::Point3d> &pointsIn3D,
                                      std::vector<cv::Point2d> &distortedPoints) const;

        protected:
            std::shared_ptr<ROpenCVDistortion> trueDistortion_;
            std::shared_ptr<ROpenCVDistortion> wrongDistortion_;
        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_ROPENCVERRDISTORTION_H

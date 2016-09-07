#ifndef RCE_GEOMETRY_RCAMERAPOSEESTIMATION_H
#define RCE_GEOMETRY_RCAMERAPOSEESTIMATION_H

#include <opencv2/core/core.hpp>

namespace rce {
    namespace geometry {

        class RCameraPoseEstimation
        {
        public:
            RCameraPoseEstimation(){}

            // FIXME: If leftHandedCoordinateSystem == true, the rotation is weird.

            static void estimateCameraPose(const cv::Mat &cameraMatrix,
                                           const cv::Mat &homographyFromWorldToImage,
                                           const cv::Point3d &pointInFrontOfCamera,
                                           std::vector<cv::Point3d> &positions,
                                           std::vector<cv::Mat> &rotations,
                                           std::vector<cv::Point3d> &headingVectors,
                                           std::vector<cv::Point3d> &toSceneVectors,
                                           bool leftHandedCoordinateSystem = false,
                                           bool filterSolutions = true,
                                           bool useAllMethods = false);

            // A "robust" version of algorithm.
            // Calculates estimates using Theia library and returns pose that is closest
            // to NADIR point (in XY plane).
            // in general, this should return just one solution, but if something wrong happens,
            // it calls estimateCameraPose with filterSolutions = true and useAllMethods = true
            static void estimateCameraPoseRobust(const cv::Mat &cameraMatrix,
                                                 const cv::Mat &homographyFromWorldToImage,
                                                 const cv::Point3d &pointInFrontOfCamera,
                                                 std::vector<cv::Point3d> &positions,
                                                 std::vector<cv::Mat> &rotations,
                                                 std::vector<cv::Point3d> &headingVectors,
                                                 std::vector<cv::Point3d> &toSceneVectors,
                                                 bool leftHandedCoordinateSystem = false);

            static cv::Mat getCleanHomography(const cv::Mat &cameraMatrix,
                                              const cv::Mat &fromWorldToImage,
                                              bool flipY = false);

            // this usually returns two identical solutions, which are somewhat simillar to solution returned by Theia
            static void estimatePoseNADIRApproach(const cv::Mat &cameraMatrix,
                                                  const cv::Mat &fromWorldToImage,
                                                  std::vector<cv::Point3d> &positions,
                                                  std::vector<cv::Mat> &rotations,
                                                  bool flipY = false);

            static void estimatePoseNADIRApproach(const cv::Mat &cleanHomographyFromWorldToImage,
                                                  std::vector<cv::Point3d> &positions,
                                                  std::vector<cv::Mat> &rotations);


            static void estimatePoseBetweenSimple(const cv::Mat &homography,
                                     std::vector<cv::Point3d> &positions,
                                     std::vector<cv::Mat> &rotations);


            static void estimatePoseBetweenDumb(const cv::Mat &homography,
                                     std::vector<cv::Point3d> &positions,
                                     std::vector<cv::Mat> &rotations);


            static void estimatePoseBetweenCVHomography(const cv::Mat &homography,
                                     std::vector<cv::Point3d> &positions,
                                     std::vector<cv::Mat> &rotations);


            static void estimatePoseBetweenImagesCVHomography(const cv::Mat &homography,
                                                              const cv::Mat &cameraMatrix,
                                     std::vector<cv::Point3d> &positions,
                                     std::vector<cv::Mat> &rotations);

            // this version is hugelly experimental hack and expects that sceneCenter is at the ground and in front of camera.
            static void estimatePoseFromHomographyTheiaP3P(const cv::Mat &homography,
                                                        const cv::Point3d &sceneCenter,
                                                        std::vector<cv::Point3d> &positions,
                                                        std::vector<cv::Mat> &rotations,
                                                           double featurePointScale = 1);
            // this version is hugelly experimental hack and expects that sceneCenter is at the ground and in front of camera.
            static void estimatePoseFromHomographyTheiaPNP(const cv::Mat &homography,
                                                        const cv::Point3d &sceneCenter,
                                                        std::vector<cv::Point3d> &positions,
                                                        std::vector<cv::Mat> &rotations,
                                                           double featurePointScale = 1);


            // the solutions will be appended to vectors. The vectors will not be cleared prior the algorithm
            static void getReasonableSolutions(const std::vector<cv::Point3d> &allPositions, // position must be above the ground
                                        const std::vector<cv::Mat> &allRotations, // determinant of rotation must be +1, the camera should be heading towards the scene
                                        const cv::Point3d &pointInFrontOfCamera,
                                        std::vector<cv::Point3d> &positions,
                                        std::vector<cv::Mat> &rotations,
                                        std::vector<cv::Point3d> &headingVectors,
                                        std::vector<cv::Point3d> &toSceneVectors,
                                               double determinantLimit = 0.1); // negative means no check
        };

    } // namespace geometry
} // namespace rce

#endif // RCE_GEOMETRY_RCAMERAPOSEESTIMATION_H

#ifndef RCE_ACCURACY_RVIRTUALSCENE_H
#define RCE_ACCURACY_RVIRTUALSCENE_H

#include <memory>

#include <QString>

#include <opencv2/core/core.hpp>

class QSettings;

namespace rce {
    namespace accuracy {

        class RVirtualCamera;

        class RVirtualScene
        {
        public:

            static std::shared_ptr<RVirtualScene> createFromSettings(QSettings &settings,
                                                              const QString &sectionName = QString());

            RVirtualScene(std::shared_ptr<RVirtualCamera> camera,
                          const cv::Point3d &pointOfView,
                          const std::vector<cv::Point2d> &landMarks,
                          const std::vector<cv::Point2d> &targets,
                          const std::vector<double> &landMarkElevationErrors,
                          const std::vector<double> &targetElevationErrors);

            const std::vector<cv::Point2d> &getLandmarks() const {return landmarks_;}
            const std::vector<cv::Point2d> &getTargets() const {return targets_;}


            void captureLandmarks(std::vector<cv::Point2d> &imagedLandmarks,
                                  std::vector<uchar> &validFlags,
                                  bool enableDistortion = true);
            void captureTargets(std::vector<cv::Point2d> &imagedTargets,
                                std::vector<uchar> &validFlags,
                                bool enableDistortion = true,
                                const std::vector<uchar> &captureFlags = std::vector<uchar>());


            int getTargetsCount() const {return targets_.size();}
            int getLandmarksCount() const {return landmarks_.size();}
            const cv::Point3d &getPointOfView() const {return pointOfView_;}

            std::shared_ptr<const RVirtualCamera> getCamera() const {return camera_;}

            cv::Point2d calculateLandmarkCentre() const;

        protected: // methods
            void capturePointsOld(const std::vector<cv::Point2d> &points,
                                  const std::vector<double> &elevationNoise,
                               std::vector<cv::Point2d> &imagedPoints,
                               std::vector<uchar> &validFlags,
                               bool enableDistortion = true,
                               std::vector<uchar> captureFlags = std::vector<uchar>());

            // does not work right...
            void capturePointsNew(const std::vector<cv::Point2d> &points,
                               std::vector<cv::Point2d> &imagedPoints,
                               std::vector<uchar> &validFlags,
                               bool enableDistortion = true,
                               const std::vector<uchar> &captureFlags = std::vector<uchar>());
        protected: // attributes
            // objects in the scene
            // ...in 3D
            std::shared_ptr<RVirtualCamera> camera_;
            cv::Point3d pointOfView_; // where the camera looks at
            // ...in 2D
            std::vector<cv::Point2d> landmarks_;
            std::vector<double> landMarkElevationErrors_;
            std::vector<cv::Point2d> targets_;
            std::vector<double> targetElevationErrors_;




        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_RVIRTUALSCENE_H

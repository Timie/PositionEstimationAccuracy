#ifndef RCE_ACCURACY_RREALEXPERIMENTANALYZER_H
#define RCE_ACCURACY_RREALEXPERIMENTANALYZER_H

#include <vector>
#include <memory>
#include <map>

#include <QString>
#include <QVector>

#include <opencv2/core/core.hpp>

class QSettings;

namespace rce {
    namespace accuracy {
        class RStatisticsCollector;

        class RRealExperimentAnalyzer
        {
        public:
            RRealExperimentAnalyzer();



            void loadSettings(QSettings &settings,
                              const QString &sectionName = QString());

            void setCameraParameters(const cv::Mat &camMatrix);

            void setData(const std::vector<cv::Point2d>  &worldPoints,
                         const std::vector<std::vector<cv::Point2d>> &imagePoints,
                         const std::vector<std::vector<uchar>> &annotFlags,
                         const QVector<cv::Mat> &images);


            void performAnalysis();


        private: // methods
            static bool allElementsUnique(const std::vector<int> &v);
            void estimateCameraPosition(const std::vector<cv::Point2d> &worldPoints,
                                        const std::vector<cv::Point2d> &imagePoints,
                                        int i);

            static void addToAvgMap(std::map<double,std::tuple<int,double,double,double>> &avgMap,
                                    double key,
                                    double error,
                                    double meanSigma,
                                    double trueSigma);

            static QString avgMapToStringCSV(const std::map<double,std::tuple<int,double,double,double>> &avgMap);


            static cv::Point2d calculateLandmarkCentre(const std::vector<cv::Point2d> &pts);



            static QString vector3dToCSV(const QVector<cv::Point3d> &vec);
        private: // attributes

            // data
            std::vector<cv::Point2d> worldPoints_;

            std::vector<std::vector<cv::Point2d>> imagePoints_;
            std::vector<std::vector<uchar>> annotationFlags_;
            QVector<cv::Mat> images_;

            // parameters
            bool randomLandmarks_;
            int numOfRandomTrials_;
            std::vector<std::vector<int>> landmarksIndices_;
            cv::Point2d principalPoint_;
            double fx_;
            double fy_;

            // results - per landmark configuraitons
            std::vector<std::vector<std::shared_ptr<RStatisticsCollector>>> targetAvgErrCollectorsPerLandmark_;
            std::vector<std::vector<std::shared_ptr<RStatisticsCollector>>> targetXCollectorsPerLandmark_;
            std::vector<std::vector<std::shared_ptr<RStatisticsCollector>>> targetYCollectorsPerLandmark_;

            // results - per image
            std::vector<std::vector<std::shared_ptr<RStatisticsCollector>>> targetAvgErrCollectorsPerImage_;
            std::vector<std::vector<std::shared_ptr<RStatisticsCollector>>> targetXCollectorsPerImage_;
            std::vector<std::vector<std::shared_ptr<RStatisticsCollector>>> targetYCollectorsPerImage_;

            // total results
            std::vector<std::shared_ptr<RStatisticsCollector>> globTargetAvgErrCollectors_;
            std::vector<std::shared_ptr<RStatisticsCollector>> globTargetXCollectors_;
            std::vector<std::shared_ptr<RStatisticsCollector>> globTargetYCollectors_;
        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_RREALEXPERIMENTANALYZER_H

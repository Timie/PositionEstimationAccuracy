#ifndef RCE_ACCURACY_RVISIONEXPERIMENT_H
#define RCE_ACCURACY_RVISIONEXPERIMENT_H

#include <vector>
#include <memory>
#include <map>

#include <QString>

#include <opencv2/core/core.hpp>

class QSettings;


namespace rce {
    namespace uncertainty
    {
        class RVectorUncertaintyGenerator;
        class RUncertaintyGenerator;
    }

    namespace accuracy {
        class RVirtualScene;
        class RStatisticsCollector;




        class RVisionExperiment
        {
        public:
            RVisionExperiment();

            void loadSettings(QSettings &settings,
                              const QString &sectionName = QString());


            void setIdentificationString(const QString& str) {idString_ = str;}

            bool run(const std::vector<uchar> &toCaptureFlags = std::vector<uchar>());

            bool analyzeNoisefreeScene();
            bool performSingleExperiment();
            bool performExperiments(int numOfExperiments);

            bool outputFindings();
            double getAvgError() const {return avgError_;}

            int getTargetsUnder02() const {return targetsUnder02_;}
            int getTargetsUnder03() const {return targetsUnder03_;}
            int getTargetsUnder04() const {return targetsUnder04_;}
            int getTargetsUnder05() const {return targetsUnder05_;}

            const std::vector<uchar> &getTargetCaptureFlags() const {return targetCaptureFlags_;}

        protected: // methods
            template<typename T>
            static void extractValidElements(const std::vector<T> &allVec,
                                      const std::vector<uchar> &flags,
                                      std::vector<T> &extractedVec);


            static void addToAvgMap(std::map<double,std::tuple<int,double,double,double>> &avgMap,
                                    double key,
                                    double error,
                                    double meanSigma,
                                    double trueSigma);


            static QString avgMapToStringCSV(const std::map<double,std::tuple<int,double,double,double>> &avgMap);


            static QString vector3dToCSV(const QVector<cv::Point3d> &vec);

        protected: // attributes
            QString idString_;

            // generators of uncertainty ("noisers?")
            // rce::uncertainty::RUncertaintyGenerator* distortionCoeffNoiser_; // for image geometrical distortion
            std::shared_ptr<rce::uncertainty::RUncertaintyGenerator> tiePointWorldErrorNoiser_; // for errors made by defining tie points in map (by a user)
            std::shared_ptr<rce::uncertainty::RUncertaintyGenerator> tiePointImageErrorNoiser_; // for errors made by localisation of tie points in image (by a user or a computer algorithm)
            std::shared_ptr<rce::uncertainty::RUncertaintyGenerator> tiePointWorldElevationNoiser_; // to simulate uneven surface
            std::shared_ptr<rce::uncertainty::RUncertaintyGenerator> targetImageErrorNoiser_; // for errors made by localisation of targets in image (by a user or a computer algorithm)
            std::shared_ptr<rce::uncertainty::RUncertaintyGenerator> targetWorldElevationNoiser_;

            // for each target, its error from "unnoised" scene to see numerical/rounding errors effect
            std::vector<double> targetsNumericErrors_;
            // for each target, an uncertainty collector, which produces necessary statistics (an histogram...)
            std::vector<std::shared_ptr<RStatisticsCollector>> targetAvgErrCollectors_;
            std::vector<std::shared_ptr<RStatisticsCollector>> targetXCollectors_;
            std::vector<std::shared_ptr<RStatisticsCollector>> targetYCollectors_;


            // scene
            std::shared_ptr<RVirtualScene> scene_;

            // experiment parameters
            int experimentsCount_;
            int minDataCount_;
            double minTargetDistFromSide_;

            // computed data
            std::vector<cv::Point2d> targetTrueImagePositions_;
            std::vector<uchar> targetCaptureFlags_;
            std::vector<double> targetDistFromSide_;

            // final results
            double avgError_;
            int targetsUnder02_;
            int targetsUnder03_;
            int targetsUnder04_;
            int targetsUnder05_;
        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_RVISIONEXPERIMENT_H

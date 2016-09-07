#ifndef RCE_ACCURACY_RDISTANCEMATRIX_H
#define RCE_ACCURACY_RDISTANCEMATRIX_H

#include <opencv2/core/core.hpp>

class QDomElement;

namespace rce {
    namespace accuracy {

        class RDistanceMatrix
        {
        public:
            RDistanceMatrix();
            RDistanceMatrix(int numOfPlaces);


            void readFromQDomElement(const QDomElement &element);
            void writeToQDomElement(QDomElement &element) const;

            void setDistance(int placeA,
                             int placeB,
                             double distance);

            void resize(int numOfPlaces);
            int getNumOfPlaces() const {return distances_.cols;}

            std::vector<cv::Point2d> generatePositions(const cv::Point2d &place1Pos = cv::Point2d(0,0),
                                                       double headingToPlace2 = 0);


            std::vector<cv::Point2d> generatePositions3Points(const cv::Point2d &place1Pos = cv::Point2d(0,0),
                                                       double headingToPlace2 = 0,
                                                       bool flippedPlace3 = false);

            bool allDistancesAreKnown() const;

            bool isDistanceKnown(int place1,
                                 int place2) const;
            double getDistance(int place1,
                               int place2) const;

            void clearValue(int place1,
                            int place2);


        protected:
            static bool isSolutionFound(const std::vector<uchar> &calculatedFlags);

        protected:
            cv::Mat distances_;
            cv::Mat setFlags_;


        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_RDISTANCEMATRIX_H

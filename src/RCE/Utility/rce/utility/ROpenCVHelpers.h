#ifndef RCE_UTILITY_ROPENCVHELPERS_H
#define RCE_UTILITY_ROPENCVHELPERS_H

#include <QTextStream>

#include <opencv2/core/core.hpp>

class QPolygonF;

namespace rce {
    namespace utility {

        //cv::Point rectangleCentre(const cv::Rect &r);

        bool rectanglesAreOverlapping(const cv::Rect &r1,
                                      const cv::Rect &r2);

        double getRectangleDifference(const cv::Rect &r1,
                                      const cv::Rect &r2);

        void getListOfNonZeroPoints(const cv::Mat &input,
                                    std::vector<cv::Point2d> &output);

        void drawPolygonOnCvMat(const QPolygonF &polygon,
                                cv::Mat &image,
                                const cv::Scalar &value,
                                const cv::Mat &transform = cv::Mat());

        // only for CV_8UC1
        int countNonZeroElems(const cv::Mat &mask);

        // expected CV_8UC1
        double absDiffAvg(const cv::Mat &mat1,
                           const cv::Mat &mat2,
                           const cv::Mat &mask);


        bool isMatEqual(const cv::Mat &mat1,
                        const cv::Mat &mat2);

        // emty matrix is considered as identity
        bool isMatEqualEmptyIdentity(const cv::Mat &mat1,
                               const cv::Mat &mat2);

        bool isPointInsideImage(const cv::Point2i &pt,
                                const cv::Mat &image);

        void copyImageOverOther(const cv::Mat &srcImage,
                                const cv::Point2i &offset,
                                cv::Mat &dstImage);


        bool prepareOverlappingParts(const cv::Mat &img1,
                                     const cv::Point2i &img1Offset,
                                     const cv::Mat &img2,
                                     cv::Mat &img1Overlap,
                                     cv::Mat &img2Overlap,
                                     cv::Point2i &img1OverOffset);

        class ROpenCVHelpers
        {
        public:
            ROpenCVHelpers();
        };

    } // namespace utility
} // namespace rce


// serialisation/deserialisation of cv::Point_<> structures

template <typename T>
QTextStream& operator<<(QTextStream& out,
                        const cv::Point_<T>& pt)
{
    out.setRealNumberPrecision(std::numeric_limits<T>::max_digits10);
    out << pt.x << ";" << pt.y;

    return out;
}


template <typename T>
QTextStream& operator>>(QTextStream& in,
                        cv::Point_<T>& pt)
{
    QChar dummy;
    in >> pt.x >> dummy >> pt.y;
    return in;
}

#endif // RCE_UTILITY_ROPENCVHELPERS_H

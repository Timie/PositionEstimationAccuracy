#ifndef RCE_UTILITY_ROPENCVTOQDEBUG_H
#define RCE_UTILITY_ROPENCVTOQDEBUG_H

class QDebug;
namespace cv
{
    class Mat;
}

//namespace rce {
//    namespace utility {

//        class ROpenCVtoQDebug
//        {
//        public:
//            ROpenCVtoQDebug();

//        signals:

//        public slots:
//        };

//    } // namespace utility
//} // namespace rce

/**
  * Operators for streaming cv::Mat into QDebug streams.
  */
QDebug operator<< (QDebug d, const cv::Mat &mat);

#endif // RCE_UTILITY_ROPENCVTOQDEBUG_H

#ifndef RCE_UTILITY_RIMAGECONVERSION_H
#define RCE_UTILITY_RIMAGECONVERSION_H


//#include <QImage>
//#include <opencv2/core/core.hpp>

class QImage;
namespace cv {class Mat;}

namespace rce {
    namespace utility {

        QImage opencvToQImage(const cv::Mat &imageCV);
        QImage convertCVUcharToQImage(const cv::Mat &imageCV);
        QImage convertCVFloatToQImage(const cv::Mat &imageCV);

        /**
         * Converts QImage to cv::Mat. Returns empty cv::Mat on failure.
         * @param reqMatType is used as a hint to define the output type
         * of the resulting cv::Mat object. In some cases the resulting
         * cv::Mat may not be the given type. When reqMatType is negative,
         * no assumption can be made about the hint.
         *
         * In case 3/4-channel image is returned, RGB/RGBA color order is returned.
         */
        cv::Mat qImageToOpencv(const QImage &image,
                               int reqMatType = -1);


        cv::Mat convertToGray(const cv::Mat &img);

        cv::Mat convertToColor(const cv::Mat &img);


//        class RImageConversion
//        {
//        public:
//            RImageConversion();
//        };

    } // namespace utility
} // namespace rce

#endif // RCE_UTILITY_RIMAGECONVERSION_H

#include "RImageConversion.h"

#include <QImage>
#include <QPainter>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/highgui/highgui.hpp>

//#define RCE_USE_OURS_IMAGE_CONVERSION

#ifndef RCE_USE_OURS_IMAGE_CONVERSION
#include "cvmatandqimage.h"
#endif


QImage
rce::utility::
opencvToQImage(const cv::Mat &imageCV)
{

#ifndef RCE_USE_OURS_IMAGE_CONVERSION
    return QtOcv::mat2Image(imageCV);
#else
    if(imageCV.empty())
        return QImage();

    switch(imageCV.depth())
    {
    case CV_8U:
    case CV_8S:
    {
        return convertCVUcharToQImage(imageCV);
        break;
    }

    case CV_32F:
    {
        return convertCVFloatToQImage(imageCV);
        break;
    }

    default:
        return QImage();

    }
#endif
}

QImage rce::utility::convertCVUcharToQImage(const cv::Mat &imageCV)
{
    cv::Mat destinationImg;

    if(imageCV.channels() == 3)
    {
        cv::cvtColor(imageCV, destinationImg, CV_BGR2RGB);
    }
    else if(imageCV.channels() == 1)
    {
        cv::cvtColor(imageCV, destinationImg, CV_GRAY2RGB);
    }
    else
    {
        std::vector <cv::Mat> channels;
        channels.resize(imageCV.channels());

        cv::split(imageCV, channels);

        std::vector <cv::Mat> toMerge;
        toMerge.resize(3, channels[0]);
        cv::merge(toMerge, destinationImg);
    }

    //IplImage tmpIplImg = destinationImg.operator IplImage();
     QImage tempImage((uchar*) destinationImg.data,
                      destinationImg.cols,
                      destinationImg.rows,
                      destinationImg.step,
                      QImage::Format_RGB888);
    //QImage tempImage((unsigned char *)tmpIplImg.imageDataOrigin,tmpIplImg.width,tmpIplImg.height,QImage::Format_RGB888);
    QImage final1 = tempImage.convertToFormat(QImage::Format_RGB32,
                              Qt::AutoColor);
    return final1;

//    QImage final(destinationImg.cols,
//                 destinationImg.rows,
//                 QImage::Format_RGB32);
//    QPainter painter(&final);
//    painter.drawImage(QPoint(0,0), tempImage);

//    return final;
}

QImage rce::utility::convertCVFloatToQImage(const cv::Mat &imageCV)
{
    cv::Mat destinationImage;

    imageCV.convertTo(destinationImage, CV_8U, 255, 0);

    return convertCVUcharToQImage(destinationImage);
}


cv::Mat rce::utility::convertToGray(const cv::Mat &img)
{
    switch(img.channels())
    {
        case 1:
            {
                return img;
            }
        case 3:
        case 4:
            {
                cv::Mat gray;
                cv::cvtColor(img, gray,
                             CV_BGR2GRAY);
                return gray;
            }
        default:
            return cv::Mat();
    }
}

cv::Mat rce::utility::convertToColor(const cv::Mat &img)
{
    switch(img.channels())
    {
        case 1:
            {
                cv::Mat colorImg;
                cv::cvtColor(img, colorImg,
                             CV_GRAY2BGR);
                return colorImg;
            }
        case 3:
            {
                return img;
            }
        case 4:
            {
                cv::Mat colorImg;
                cv::cvtColor(img, colorImg,
                             CV_RGBA2RGB);
                return colorImg;
            }
        default:
            return cv::Mat();
    }
}


cv::Mat
rce::utility::
qImageToOpencv(const QImage &image,
               int reqMatType)
{
    if(reqMatType < 0)
    {
        reqMatType = CV_8UC(3);
    }

#ifndef RCE_USE_OURS_IMAGE_CONVERSION
    return QtOcv::image2Mat(image,
                            reqMatType,
                            QtOcv::MCO_RGB);
#else
    QImage bgrImage = image.convertToFormat(QImage::Format_RGB888); // NOTE: Maybe BGR
    cv::Mat mat(rgbImage.size().height(),
                rgbImage.size().width(),
                CV_8UC3,
                rgbImage.bits(),
                rgbImage.bytesPerLine());
    cv::Mat result;
    cv::cvtColor(mat, result, CV_RGB2BGR);
    return result;
#endif
}


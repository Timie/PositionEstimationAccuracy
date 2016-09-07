#include "ROpenCVtoQDebug.h"

#include <sstream>

#include <QDebug>
#include <QString>

#include <opencv2/core/core.hpp>

QDebug
operator<<(QDebug d,
           const cv::Mat &mat)
{
    std::stringstream stream;
    stream << mat;
    d << stream.str().c_str();

    return d;
}

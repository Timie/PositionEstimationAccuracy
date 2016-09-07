#include "RXmlConversion.h"

#include <QDomElement>
#include <QDomDocument>
#include <QPolygonF>

#include <opencv2/core/core.hpp>


void
rce::utility::RXmlConversion::
writeToQDomElement(QDomElement &element,
                   const QVector<QPointF> &polygon)
{

    QDomDocument ownerDocument = element.ownerDocument();

    element.setAttribute("size", polygon.size());
    for(int i = 0; i < polygon.size(); ++i)
    {
        QDomElement pointElement = ownerDocument.createElement(QString("point_")+QString::number(i));
        pointElement.setAttribute("x", polygon[i].x());
        pointElement.setAttribute("y", polygon[i].y());
        element.appendChild(pointElement);
    }
}

void
rce::utility::RXmlConversion::
readFromQDomElement(const QDomElement &element,
                    QVector<QPointF> &polygon)
{
    quint32 shapeSize = element.attribute("size").toUInt();

    double buffer;
    polygon.resize(shapeSize);
    for(size_t i = 0; i < shapeSize; ++i)
    {
        QDomElement pointElement = element.firstChildElement(QString("point_")+QString::number(i));
        /*polygon[i].setX(pointElement.attribute("x").toFloat()); // FIXME: Test out with toDouble
        polygon[i].setY(pointElement.attribute("y").toFloat());*/

        // this version does not lose precision:
        QString attributeX = pointElement.attribute("x");
        QTextStream tsX(&attributeX, QIODevice::ReadOnly);
        tsX >> buffer;
        polygon[i].setX(buffer);

        QString attributeY = pointElement.attribute("y");
        QTextStream tsY(&attributeY, QIODevice::ReadOnly);
        tsY >> buffer;
        polygon[i].setY(buffer);
    }
}

void
rce::utility::RXmlConversion::
writeToQDomElement(QDomElement &element,
                   const QVector<QRectF> &rectVector)
{
    QDomDocument ownerDocument = element.ownerDocument();

    element.setAttribute("size", rectVector.size());
    for(int i = 0; i < rectVector.size(); ++i)
    {
        QDomElement rectElement = ownerDocument.createElement(QString("rect_")+QString::number(i));
        rectElement.setAttribute("x", rectVector[i].x());
        rectElement.setAttribute("y", rectVector[i].y());
        rectElement.setAttribute("w", rectVector[i].width());
        rectElement.setAttribute("h", rectVector[i].height());
        element.appendChild(rectElement);
    }
}

void
rce::utility::RXmlConversion::
readFromQDomElement(const QDomElement &element,
                    QVector<QRectF> &rectVector)
{
    quint32 shapeSize = element.attribute("size").toUInt();

    // double buffer;
    rectVector.resize(shapeSize);
    for(size_t i = 0; i < shapeSize; ++i)
    {
        QDomElement rectElement = element.firstChildElement(QString("rect_")+QString::number(i));
        rectVector[i].setX(rectElement.attribute("x").toDouble());
        rectVector[i].setY(rectElement.attribute("y").toDouble());
        rectVector[i].setWidth(rectElement.attribute("w").toDouble());
        rectVector[i].setHeight(rectElement.attribute("h").toDouble());

        // this version does not lose precision:
//        QString attributeX = rectElement.attribute("x");
//        QTextStream tsX(&attributeX, QIODevice::ReadOnly);
//        tsX >> buffer;
//        polygon[i].setX(buffer);

//        QString attributeY = rectElement.attribute("y");
//        QTextStream tsY(&attributeY, QIODevice::ReadOnly);
//        tsY >> buffer;
//        polygon[i].setY(buffer);
    }
}

void
rce::utility::RXmlConversion::
writeToQDomElement(QDomElement &element,
                   const cv::Mat &matrix)
{

    element.setAttribute("matrix_type", matrix.type());
    element.setAttribute("matrix_depth", matrix.depth());
    element.setAttribute("matrix_channels", matrix.channels());
    element.setAttribute("matrix_cols", matrix.cols);
    element.setAttribute("matrix_rows", matrix.rows);

    switch(matrix.depth())
    {
        case CV_8U:
            writeMatDepthToQDomElement<quint8>(element, matrix);
            break;
        case CV_8S:
            writeMatDepthToQDomElement<qint8>(element, matrix);
            break;
        case CV_16U:
            writeMatDepthToQDomElement<quint16>(element, matrix);
            break;
        case CV_16S:
            writeMatDepthToQDomElement<qint16>(element, matrix);
            break;
        case CV_32S:
            writeMatDepthToQDomElement<qint32>(element, matrix);
            break;
        case CV_32F:
            writeMatDepthToQDomElement<float>(element, matrix);
            break;
        case CV_64F:
            writeMatDepthToQDomElement<double>(element, matrix);
            break;
        default:
            throw QString("RXmlConversion::writeToQDomElement: Unsupported matrix depth.");
            break;
    }
}

template <typename ScalarType>
void
rce::utility::RXmlConversion::
writeMatDepthToQDomElement(QDomElement &element,
                      const cv::Mat &matrix)
{
    switch(matrix.channels())
    {
        case 1:
            writeMatDepthChannelToQDomElement<ScalarType, 1>(element, matrix);
            break;
        case 2:
            writeMatDepthChannelToQDomElement<ScalarType, 2>(element, matrix);
            break;
        case 3:
            writeMatDepthChannelToQDomElement<ScalarType, 3>(element, matrix);
            break;
        case 4:
            writeMatDepthChannelToQDomElement<ScalarType, 4>(element, matrix);
            break;
        default:
            throw QString("RXmlConversion::writeToQDomElement: Unsupported matrix number of channels.");
            break;
    }
}

template <typename ElemType, int CHANNELS>
void
rce::utility::RXmlConversion::
writeMatDepthChannelToQDomElement(QDomElement &element,
                                  const cv::Mat &matrix)
{
    QDomDocument ownerDocument = element.ownerDocument();

    typedef cv::Vec<ElemType, CHANNELS> AccessVectorType;
    for(int y = 0; y < matrix.rows; ++y)
    {
        QDomElement rowElement = ownerDocument.createElement(QString("row_%1").arg(y));

        for(int x = 0; x < matrix.cols; ++x)
        {
            AccessVectorType value = matrix.at<AccessVectorType>(y,x);
            if(CHANNELS == 1)
            {
                rowElement.setAttribute(QString("val_%1").arg(x), value(0));
            }
            else
            {
                for(int c = 0; c < CHANNELS; ++c)
                {
                    rowElement.setAttribute(QString("val_%1_%2").arg(x).arg(c), value(c));
                }
            }
        }
        element.appendChild(rowElement);
    }
}

void
rce::utility::RXmlConversion::
readFromQDomElement(const QDomElement &element, cv::Mat &matrix)
{
    int matrixType = element.attribute("matrix_type").toInt();
    int matrixDepth = element.attribute("matrix_depth").toInt();
    //int matrixChannels = element.attribute("matrix_channels").toInt();
    int matrixCols = element.attribute("matrix_cols").toInt();
    int matrixRows = element.attribute("matrix_rows").toInt();

    matrix = cv::Mat(matrixRows, matrixCols, matrixType);

    switch(matrixDepth)
    {
        case CV_8U:
            readMatDepthFromQDomElement<quint8, unsigned short>(element, matrix);
            break;
        case CV_8S:
            readMatDepthFromQDomElement<qint8, short >(element, matrix);
            break;
        case CV_16U:
            readMatDepthFromQDomElement<quint16, quint16>(element, matrix);
            break;
        case CV_16S:
            readMatDepthFromQDomElement<qint16, qint16>(element, matrix);
            break;
        case CV_32S:
            readMatDepthFromQDomElement<qint32, qint32>(element, matrix);
            break;
        case CV_32F:
            readMatDepthFromQDomElement<float, float>(element, matrix);
            break;
        case CV_64F:
            readMatDepthFromQDomElement<double, double>(element, matrix);
            break;
        default:
            throw QString("RXmlConversion::readFromQDomElement: Unsupported matrix depth.");
            break;
    }
}

template <typename ScalarType, typename DeserializingType>
void
rce::utility::RXmlConversion::
readMatDepthFromQDomElement(const QDomElement &element, cv::Mat &matrix)
{
    switch(matrix.channels())
    {
        case 1:
            readMatDepthChannelFromQDomElement<ScalarType, DeserializingType, 1>(element, matrix);
            break;
        case 2:
            readMatDepthChannelFromQDomElement<ScalarType, DeserializingType, 2>(element, matrix);
            break;
        case 3:
            readMatDepthChannelFromQDomElement<ScalarType, DeserializingType, 3>(element, matrix);
            break;
        case 4:
            readMatDepthChannelFromQDomElement<ScalarType, DeserializingType, 4>(element, matrix);
            break;
        default:
            throw QString("RXmlConversion::writeToQDomElement: Unsupported matrix number of channels.");
            break;
    }
}

template <typename ElemType, typename DeserializingType, int CHANNELS>
void
rce::utility::RXmlConversion::
readMatDepthChannelFromQDomElement(const QDomElement &element, cv::Mat &matrix)
{
    typedef cv::Vec<ElemType, CHANNELS> AccessVectorType;
    DeserializingType buffer;
    for(int y = 0; y < matrix.rows; ++y)
    {
        QDomElement rowElement = element.firstChildElement(QString("row_%1").arg(y));

        for(int x = 0; x < matrix.cols; ++x)
        {
            AccessVectorType value;
            if(CHANNELS == 1)
            {
                QString attribute = rowElement.attribute(QString("val_%1").arg(x));
                QTextStream ts(&attribute, QIODevice::ReadOnly);
                ts >> buffer;
                value[0] = buffer;
            }
            else
            {
                for(int c = 0; c < CHANNELS; ++c)
                {
                    QString attribute = rowElement.attribute(QString("val_%1_%2").arg(x).arg(c));
                    QTextStream ts(&attribute, QIODevice::ReadOnly);
                    ts >> buffer;
                    value[c] = buffer;
                }
            }

            matrix.at<AccessVectorType>(y,x) = value;
        }
    }
}


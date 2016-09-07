#ifndef RCE_UTILITY_RXMLCONVERSION_H
#define RCE_UTILITY_RXMLCONVERSION_H

#include <QVector>

#include <QDomElement>
#include <QDomDocument>
#include <QStringList>
#include <QTextStream>

#include <QDebug>

#include <opencv2/core/core.hpp>

class QPolygonF;

namespace cv { class Mat;}


// NOTE: Stream operators for cv::Point_ are implemented in ROpenCVHelpers.h

namespace rce {
    namespace utility {        

        class RXmlConversion
        {
        public:
            RXmlConversion();

            static void writeToQDomElement(QDomElement &element, const QVector<QPointF> &polygon);
            static void readFromQDomElement(const QDomElement &element, QVector<QPointF> &polygon);


            static void writeToQDomElement(QDomElement &element, const QVector<QRectF> &rectVector);
            static void readFromQDomElement(const QDomElement &element, QVector<QRectF> &rectVector);


            static void writeToQDomElement(QDomElement &element, const cv::Mat &matrix);
            static void readFromQDomElement(const QDomElement &element, cv::Mat &matrix);

            template <typename VecType>
            static void writeToQDomElement(QDomElement &element, const QVector<VecType> &vec)
            {
                QDomDocument ownerDocument = element.ownerDocument();

                element.setAttribute("vector_size", vec.size());

                QString textData;
                {
                    QTextStream textDataStream(&textData, QIODevice::WriteOnly);
                    textDataStream.setRealNumberPrecision(17);
                    for(int i = 0; i < vec.size(); ++i)
                    {
                    //textData.append(QString("%1 ").arg(vec[i]));
                        textDataStream << vec[i] << " ";
                    }
                }



                QDomText textDataNode = ownerDocument.createTextNode(textData);
                element.appendChild(textDataNode);

            }


            template <typename VecType>
            static void readFromQDomElement(const QDomElement &element, QVector<VecType> &vec)
            {
                int vecSize = element.attribute("vector_size").toInt();
                vec.resize(vecSize);
                QString textData = element.text();
//                if(textData.isEmpty())
//                {
//                    qDebug() << "Text data is empty";
//                }
//                else
//                {
//                    qDebug() << "First text data: \n" << textData;
//                }
//                QDomNodeList nodes = element.childNodes();
//                for(int i = 0; i < nodes.size(); ++i)
//                {
//                    if(nodes.at(i).nodeType() == QDomNode::TextNode)
//                    {
//                        textData = nodes.at(i).toText().data();
//                        break;
//                    }
//                }
//                if(textData.isEmpty())
//                {
//                    qDebug() << "Text data is still empty";
//                }
//                else
//                {
//                    qDebug() << "Second text data: \n" << textData;
//                }


                QStringList splittedTextData = textData.split(' ', QString::SkipEmptyParts);
                if(splittedTextData.size() != vecSize)
                {
                    throw QString("RXmlConversion::readFromQDomElement<QVector<VecType>>: Vector data is corrupted. VecSize: %1 SplitSize: %2").arg(vecSize).arg(splittedTextData.size());
                }

                for(int i = 0; i < splittedTextData.size(); ++i)
                {
                    QTextStream ts(&splittedTextData[i], QIODevice::ReadOnly);
                    ts >> vec[i];
                }
            }

        protected:
            template <typename ElemType>
            static void writeMatDepthToQDomElement(QDomElement &element, const cv::Mat &matrix);

            template <typename ElemType, int CHANNELS>
            static void writeMatDepthChannelToQDomElement(QDomElement &element, const cv::Mat &matrix);


            template <typename ElemType, typename DeserializingType>
            static void readMatDepthFromQDomElement(const QDomElement &element, cv::Mat &matrix);

            template <typename ElemType, typename DeserializingType, int CHANNELS>
            static void readMatDepthChannelFromQDomElement(const QDomElement &element, cv::Mat &matrix);
        };

    } // namespace utility
} // namespace rce



#endif // RCE_UTILITY_RXMLCONVERSION_H

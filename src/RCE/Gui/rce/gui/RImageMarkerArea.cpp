#include "RImageMarkerArea.h"

#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QtCore/qmath.h>

#include <QDebug>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "dfs/core/DDebug.h"

namespace rce {
    namespace gui {

        const char* RImageMarkerArea::variousColours_[] = {
            "#000000", "#FFFF00", "#1CE6FF", "#FF34FF", "#FF4A46", "#008941", "#006FA6", "#A30059",
            "#FFDBE5", "#7A4900", "#0000A6", "#63FFAC", "#B79762", "#004D43", "#8FB0FF", "#997D87",
            "#5A0007", "#809693", "#FEFFE6", "#1B4400", "#4FC601", "#3B5DFF", "#4A3B53", "#FF2F80",
            "#61615A", "#BA0900", "#6B7900", "#00C2A0", "#FFAA92", "#FF90C9", "#B903AA", "#D16100",
            "#DDEFFF", "#000035", "#7B4F4B", "#A1C299", "#300018", "#0AA6D8", "#013349", "#00846F",
            "#372101", "#FFB500", "#C2FFED", "#A079BF", "#CC0744", "#C0B9B2", "#C2FF99", "#001E09",
            "#00489C", "#6F0062", "#0CBD66", "#EEC3FF", "#456D75", "#B77B68", "#7A87A1", "#788D66",
            "#885578", "#FAD09F", "#FF8A9A", "#D157A0", "#BEC459", "#456648", "#0086ED", "#886F4C",

            "#34362D", "#B4A8BD", "#00A6AA", "#452C2C", "#636375", "#A3C8C9", "#FF913F", "#938A81",
            "#575329", "#00FECF", "#B05B6F", "#8CD0FF", "#3B9700", "#04F757", "#C8A1A1", "#1E6E00",
            "#7900D7", "#A77500", "#6367A9", "#A05837", "#6B002C", "#772600", "#D790FF", "#9B9700",
            "#549E79", "#FFF69F", "#201625", "#72418F", "#BC23FF", "#99ADC0", "#3A2465", "#922329",
            "#5B4534", "#FDE8DC", "#404E55", "#0089A3", "#CB7E98", "#A4E804", "#324E72", "#6A3A4C",
            "#83AB58", "#001C1E", "#D1F7CE", "#004B28", "#C8D0F6", "#A3A489", "#806C66", "#222800",
            "#BF5650", "#E83000", "#66796D", "#DA007C", "#FF1A59", "#8ADBB4", "#1E0200", "#5B4E51",
            "#C895C5", "#320033", "#FF6832", "#66E1D3", "#CFCDAC", "#D0AC94", "#7ED379", "#012C58"};

        RImageMarkerArea::RImageMarkerArea(QWidget *parent, Qt::WindowFlags f):
            QLabel(parent, f),
            highlightedMaskValue_(0),
            markingSize_(5),
            scale_(1),
            mouseButton_(0)
        {

            setAlignment(Qt::AlignLeft | Qt::AlignTop);
        }

        void RImageMarkerArea::setImage(const QImage &image)
        {
            bkgPixmap_ = QPixmap::fromImage(image);
            zoomedBkgPixmap_ = bkgPixmap_.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            zoomedColorMask_ = QPixmap(zoomedBkgPixmap_.size());
            zoomedColorMask_.fill(QColor(0,0,0,0));
            markerMask_ = cv::Mat::zeros(cv::Size(bkgPixmap_.width(), bkgPixmap_.height()), CV_8UC1);

            scale_ = static_cast<double>(bkgPixmap_.width()) / static_cast<double>(zoomedBkgPixmap_.width());

            updateDisplayedPixmap();
        }

        bool RImageMarkerArea::setMarkers(const cv::Mat &markerMask)
        {
            if((markerMask_.size() == cv::Size(bkgPixmap_.width(),
                                              bkgPixmap_.height())) &&
               (markerMask_.type() == CV_8UC1))
            {

                markerMask_ = markerMask;

                redrawColorMask();
                return true;
            }
            else
                return false;
        }

        void RImageMarkerArea::clearMarkers()
        {
            zoomedColorMask_ = QPixmap(zoomedBkgPixmap_.size());
            zoomedColorMask_.fill(QColor(0,0,0,0));
            markerMask_ = cv::Mat::zeros(cv::Size(bkgPixmap_.width(), bkgPixmap_.height()), CV_8UC1);

            updateDisplayedPixmap();

        }

        const cv::Mat &RImageMarkerArea::getMarkerMask() const
        {
            return markerMask_;
        }

        void RImageMarkerArea::setMarkingStyle(quint32 markSize,
                                               quint8 maskValue,
                                               const QColor &visualisationColor)
        {
            markingSize_ = markSize;
            markingValue_ = maskValue;
            QColor newColor;
            if(QColor(Qt::black) != visualisationColor)
            {
                newColor = visualisationColor;
            }
            else
            {
                newColor = QColor(variousColours_[(maskToColorMap_.size() + 1) % 128]);
            }
            if(maskToColorMap_.contains(maskValue) &&
               (newColor != maskToColorMap_[maskValue]))
            {
                maskToColorMap_[maskValue] = newColor;
                redrawColorMask();
            }
            else
            {
                maskToColorMap_[maskValue] = newColor;
            }

            currentColor_ = maskToColorMap_[maskValue];
            setCurrentColorOpacity();
        }

        void RImageMarkerArea::setMarkerSize(quint32 markSize)
        {
            markingSize_ = markSize;
        }

        void RImageMarkerArea::setHighlight(int maskValue)
        {
            highlightedMaskValue_ = maskValue;

            if(!markerMask_.empty())
            {
                setCurrentColorOpacity();
                redrawColorMask();
            }
        }

        void RImageMarkerArea::mouseMoveEvent(QMouseEvent *event)
        {
            mouseButton_ = event->buttons();

            if((mouseButton_ & Qt::LeftButton) != 0)
            {
                QPen pen = getColorPen();
                QBrush brush = getColorBrush();

                // draw color markers
                QPainter markerPainter(&zoomedColorMask_);
                markerPainter.setCompositionMode(QPainter::CompositionMode_Source);
                markerPainter.setPen(pen);
                markerPainter.setBrush(brush);
                markerPainter.drawLine(prevMousePosition_, event->pos());


                // draw on markers mask
                cv::Point unzoomedPoint1(event->pos().x() * scale_,
                                        event->pos().y() * scale_);
                cv::Point unzoomedPoint2(prevMousePosition_.x() * scale_,
                                         prevMousePosition_.y() * scale_);
                cv::line(markerMask_, unzoomedPoint1, unzoomedPoint2, cv::Scalar(markingValue_), qRound(markingSize_ * scale_));


                updateDisplayedPixmap();
                prevMousePosition_ = event->pos();

                event->accept();

            }
            else if((mouseButton_ & Qt::RightButton) != 0)
            {
                QPen pen = getColorPen();
                pen.setColor(QColor(0,0,0,0));
                QBrush brush = getColorBrush();
                brush.setColor(QColor(0,0,0,0));

                // draw color markers
                QPainter markerPainter(&zoomedColorMask_);
                markerPainter.setCompositionMode(QPainter::CompositionMode_Source);
                markerPainter.setPen(pen);
                markerPainter.setBrush(brush);
                markerPainter.drawLine(prevMousePosition_, event->pos());


                // draw on markers mask
                cv::Point unzoomedPoint1(event->pos().x() * scale_,
                                        event->pos().y() * scale_);
                cv::Point unzoomedPoint2(prevMousePosition_.x() * scale_,
                                         prevMousePosition_.y() * scale_);
                cv::line(markerMask_, unzoomedPoint1, unzoomedPoint2, cv::Scalar(0), qRound(markingSize_ * scale_));


                updateDisplayedPixmap();
                prevMousePosition_ = event->pos();

                event->accept();
            }
            else
                QLabel::mouseMoveEvent(event);
        }

        void RImageMarkerArea::mousePressEvent(QMouseEvent *event)
        {
            mouseButton_ = event->buttons();

            if((mouseButton_ & Qt::LeftButton) != 0)
            {
                prevMousePosition_ = event->pos();
                QPen pen = getColorPen();
                pen.setWidth(0);
                QBrush brush = getColorBrush();

                // draw color markers
                QPainter markerPainter(&zoomedColorMask_);
                markerPainter.setCompositionMode(QPainter::CompositionMode_Source);
                markerPainter.setPen(pen);
                markerPainter.setBrush(brush);
                markerPainter.drawEllipse(prevMousePosition_, qCeil(markingSize_ / 2.0), qCeil(markingSize_ / 2.0));

                // draw on markers mask
                cv::Point unzoomedPoint(prevMousePosition_.x() * scale_,
                                        prevMousePosition_.y() * scale_);
                cv::line(markerMask_, unzoomedPoint, unzoomedPoint, cv::Scalar(markingValue_), qRound(markingSize_ * scale_));


                updateDisplayedPixmap();

                event->accept();
            }
            else if((mouseButton_ & Qt::RightButton) != 0)
            {
                prevMousePosition_ = event->pos();
                QPen pen = getColorPen();
                pen.setColor(QColor(0,0,0,0));
                pen.setWidth(0);
                QBrush brush = getColorBrush();
                brush.setColor(QColor(0,0,0,0));

                // draw color markers
                QPainter markerPainter(&zoomedColorMask_);
                markerPainter.setCompositionMode(QPainter::CompositionMode_Source);
                markerPainter.setPen(pen);
                markerPainter.setBrush(brush);
                markerPainter.drawEllipse(prevMousePosition_, qCeil(markingSize_ / 2.0), qCeil(markingSize_ / 2.0));

                // draw on markers mask
                cv::Point unzoomedPoint(prevMousePosition_.x() * scale_,
                                        prevMousePosition_.y() * scale_);
                cv::line(markerMask_, unzoomedPoint, unzoomedPoint, cv::Scalar(0), qRound(markingSize_ * scale_));


                updateDisplayedPixmap();

                event->accept();
            }
            else
                QLabel::mousePressEvent(event);
        }

        void RImageMarkerArea::mouseReleaseEvent(QMouseEvent *event)
        {
            mouseButton_ = event->buttons();

            if((mouseButton_ & Qt::LeftButton) != 0)
            {
                mousePressEvent(event);
            }
            else if((mouseButton_ & Qt::RightButton) != 0)
            {
                mousePressEvent(event);
            }
            else
                QLabel::mouseReleaseEvent(event);
        }

        void RImageMarkerArea::resizeEvent(QResizeEvent *event)
        {
            if(event->oldSize() != size())
            {
                zoomedBkgPixmap_ = bkgPixmap_.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                zoomedColorMask_ = zoomedColorMask_.scaled(zoomedBkgPixmap_.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

                scale_ = static_cast<double>(bkgPixmap_.width()) / static_cast<double>(zoomedBkgPixmap_.width());

                dDebug() << "Scale" << scale_;

                updateDisplayedPixmap();
            }

            QLabel::resizeEvent(event);
        }

        QPen RImageMarkerArea::getColorPen()
        {
            return QPen (getColorBrush(), markingSize_, Qt::SolidLine, Qt::RoundCap);
        }

        QBrush RImageMarkerArea::getColorBrush()
        {
            return QBrush(currentColor_, Qt::SolidPattern);
        }

        void RImageMarkerArea::updateDisplayedPixmap()
        {
            QPixmap newBkg(zoomedBkgPixmap_);
            QPainter painter(&newBkg);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.setOpacity(1);
            painter.drawPixmap(QPoint(0,0),
                               zoomedColorMask_);

            setPixmap(newBkg);

            update();
        }

        void RImageMarkerArea::redrawColorMask()
        {
            {
                zoomedColorMask_.fill(QColor(0,0,0,0));
                QPainter painter(&zoomedColorMask_);
                painter.setCompositionMode(QPainter::CompositionMode_Source);

                QBrush brush = getColorBrush();
                QPen pen = getColorPen();
                pen.setWidth(0);

                painter.setBrush(brush);
                painter.setPen(pen);

                cv::Mat smallerMarkerMask;
                cv::resize(markerMask_, smallerMarkerMask, cv::Size(zoomedColorMask_.width(), zoomedColorMask_.height()), 0, 0, cv::INTER_NEAREST);

                for(int x = 0; x < smallerMarkerMask.size().width; ++x)
                {
                    for(int y = 0; y < smallerMarkerMask.size().height; ++y)
                    {
                        quint8 value = smallerMarkerMask.at<quint8>(cv::Point(x,y));
                        QColor color;
                        if(value == 0)
                        {
                            color = QColor(0,0,0,0);
                        }
                        else
                        {
                            color = getColorForMaskValue(value);

                            if(value == highlightedMaskValue_)
                                color.setAlpha(200);
                            else
                                color.setAlpha(100);
                        }


                        //brush.setColor(color);
                        //painter.setBrush(brush);
                        painter.fillRect(x,y,1,1, color);
                    }
                }

            }

            updateDisplayedPixmap();
        }

        void RImageMarkerArea::setCurrentColorOpacity()
        {
            if(markingValue_ == highlightedMaskValue_)
            {
                currentColor_.setAlpha(200);
            }
            else
            {
                currentColor_.setAlpha(100);
            }
        }

        QColor RImageMarkerArea::getColorForMaskValue(quint8 maskValue)
        {
            if(maskToColorMap_.contains(maskValue))
            {
                return maskToColorMap_[maskValue];
            }
            else
            {
                maskToColorMap_[maskValue] = QColor(variousColours_[(maskToColorMap_.size() + 1) % 128]);

                return maskToColorMap_[maskValue];
            }
        }



    } // namespace gui
} // namespace rce

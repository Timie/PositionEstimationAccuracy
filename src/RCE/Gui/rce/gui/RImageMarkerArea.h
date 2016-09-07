#ifndef RCE_GUI_RIMAGEMARKERAREA_H
#define RCE_GUI_RIMAGEMARKERAREA_H

#include <QLabel>
#include <QString>
#include <QColor>
#include <QMap>
#include <QImage>
#include <QPixmap>

#include <opencv2/core/core.hpp>

namespace rce {
    namespace gui {

        class RImageMarkerArea : public QLabel
        {
            Q_OBJECT
        public:
            RImageMarkerArea(QWidget *parent,
                             Qt::WindowFlags f = 0);

            void setImage(const QImage &image);
            bool setMarkers(const cv::Mat &markerMask);
            void clearMarkers();
            const cv::Mat &getMarkerMask() const;

            void setMarkingStyle(quint32 markSize,
                                 quint8 maskValue,
                                 const QColor &visualisationColor = QColor(Qt::black));

            void setMarkerSize(quint32 markSize);

            void setHighlight(int maskValue);

        protected slots:
            virtual void mouseMoveEvent(QMouseEvent *event);
            virtual void mousePressEvent(QMouseEvent *event);
            virtual void mouseReleaseEvent(QMouseEvent *event);

            virtual void resizeEvent(QResizeEvent *event);

        protected:

            QPen getColorPen();
            QBrush getColorBrush();

            void updateDisplayedPixmap();

            void redrawColorMask();
            void setCurrentColorOpacity();

            QColor getColorForMaskValue(quint8 maskValue);

        protected:

            QPixmap bkgPixmap_;
            QPixmap zoomedBkgPixmap_;
            QPixmap zoomedColorMask_;

            cv::Mat markerMask_;

            quint8 highlightedMaskValue_;


            quint32 markingSize_;
            quint8 markingValue_;
            QColor currentColor_;
            QMap<quint8, QColor> maskToColorMap_;
            double scale_;

            QPoint prevMousePosition_; //tracks mouse movements
            Qt::MouseButtons mouseButton_;

            static const char* variousColours_[];

        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RIMAGEMARKERAREA_H

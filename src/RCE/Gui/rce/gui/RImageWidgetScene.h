#ifndef RCE_GUI_RIMAGEWIDGETSCENE_H
#define RCE_GUI_RIMAGEWIDGETSCENE_H

#include <QGraphicsScene>

#include <QImage>

namespace cv {class Mat;}

namespace rce {
    namespace gui {

        class RImageWidgetScene : public QGraphicsScene
        {
        public:
            RImageWidgetScene(QObject *parent = NULL);

            void setBackgroundImage(const cv::Mat &image);
            void setBackgroundImage(const QImage &image);
        protected:

            virtual void drawBackground(QPainter *painter,
                                        const QRectF &rect);


        protected:
            QImage backgroundImage_;

        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RIMAGEWIDGETSCENE_H

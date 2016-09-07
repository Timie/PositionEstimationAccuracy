#ifndef RCE_GUI_RSCALABLEGRAPHICSVIEW_H
#define RCE_GUI_RSCALABLEGRAPHICSVIEW_H


#include <QGraphicsView>


namespace rce {
    namespace gui {

        class RScalableGraphicsView : public QGraphicsView
        {
            Q_OBJECT
        public:
            RScalableGraphicsView(QGraphicsScene *scene,
                                  QWidget *parent = NULL);

            virtual ~RScalableGraphicsView();

            bool canScaleIn() const;
            bool canScaleOut() const;
            int getScale() const {return currentScale_;}
            int getMinimumScale() const {return scaleMin_;}
            int getMaximumScale() const {return scaleMax_;}

            void setErrorMessage(const QString &errorMessage);

        signals:
            void scaled(int value); // value is in percents

        public slots:
            void setScale(int value/*, bool emitScaled = true*/);
            void setScaleBounds(int min,
                                int max); // value in percents
            void setScaleStep(int value);
            void scaleIn();
            void scaleOut();

            void moveViewBy(QPointF delta);

            void setUpdateBoundaryOnScale(bool value);
            void updateBoundary();

        protected slots:
            virtual void keyPressEvent(QKeyEvent *keyEvent);
            virtual void mousePressEvent(QMouseEvent *mouseEvent);
            virtual void mouseMoveEvent(QMouseEvent *mouseEvent);
            virtual void mouseReleaseEvent(QMouseEvent *mouseEvent);
            virtual void wheelEvent(QWheelEvent *event);

            virtual void drawForeground(QPainter * painter, const QRectF & rect);

        protected:
            QPoint dragLastPosition_;
            int currentScale_;
            int scaleMin_;
            int scaleMax_;
            int scaleStep_;
            bool updateBoundaryOnScale_;

            QString errorMessage_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RSCALABLEGRAPHICSVIEW_H

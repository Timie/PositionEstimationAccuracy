#ifndef RCE_GUI_RDISTANCEMATRIXSCENE_H
#define RCE_GUI_RDISTANCEMATRIXSCENE_H

#include <QGraphicsScene>
#include <QVector>

#include "rce/accuracy/RDistanceMatrix.h"

namespace rce {
    namespace gui {
        class RGraphicsFlagItem;

        class RDistanceMatrixScene : public QGraphicsScene
        {
            Q_OBJECT
        public:
            RDistanceMatrixScene(QObject *parent = NULL);


            void showPoints(const std::vector<cv::Point2d> &points,
                            const rce::accuracy::RDistanceMatrix &m);
            void clearScene();
        protected:
            QVector<RGraphicsFlagItem *> flagItems_;
            QVector<QGraphicsLineItem *> lineItems_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RDISTANCEMATRIXSCENE_H

#ifndef RCE_GUI_RGRAPHICSCROSSITEM_H
#define RCE_GUI_RGRAPHICSCROSSITEM_H

#include <QGraphicsPathItem>
#include <QPen>


namespace rce {
    namespace gui {

        class RGraphicsCrossItem : public QGraphicsPathItem
        {
        public:
            RGraphicsCrossItem(const QPointF& point,
                               const QPen &pen,
                               double size,
                               QGraphicsItem *parent = NULL);

            void setPoint(const QPointF& point) {point_ = point;}
            const QPointF &getPoint() const { return point_;}
            void setSize(double value) {size_ = value;}
            double getSize() const {return size_;}


            void regenerate();

        protected: // attributes
            QPointF point_;
            double size_;

        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RGRAPHICSCROSSITEM_H

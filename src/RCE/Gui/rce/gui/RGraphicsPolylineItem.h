#ifndef RCE_GUI_RGRAPHICSPOLYLINEITEM_H
#define RCE_GUI_RGRAPHICSPOLYLINEITEM_H

#include <QGraphicsPathItem>
#include <QPen>
#include <QPainterPath>

namespace rce {
    namespace gui {

        class RGraphicsPolylineItem : public QGraphicsPathItem
        {
        public:
            RGraphicsPolylineItem(const QPolygonF &path,
                                  const QPen &pen,
                                  QGraphicsItem *parent = nullptr);
            virtual ~RGraphicsPolylineItem() {}

            virtual QPainterPath lineShape() const;
            void regenerate();

            const QPolygonF &getPolyline() const {return polyline_;}
            void setPolyline(const QPolygonF &polygon) {polyline_ = polygon;}
            void setPenWidth(const double penWidth);

        protected:
            QPolygonF polyline_;
            QPainterPath lineShape_;

            double penWidth_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RGRAPHICSPOLYLINEITEM_H

#include "RGraphicsCrossItem.h"


rce::gui::RGraphicsCrossItem::
RGraphicsCrossItem(const QPointF &point,
                   const QPen &pen,
                   double size,
                   QGraphicsItem *parent):
    QGraphicsPathItem(parent),
    point_(point),
    size_(size)
{
    regenerate();

    setBrush(QBrush(Qt::transparent));
    setPen(pen);
}

void
rce::gui::RGraphicsCrossItem::
regenerate()
{
    QPainterPath path;
    if(!point_.isNull())
    {
        // make a cross...
        path.moveTo(point_.x() - size_/2.0,
                    point_.y() - size_/2.0);
        path.lineTo(point_.x() + size_/2.0,
                    point_.y() + size_/2.0);
        path.moveTo(point_.x() - size_/2.0,
                    point_.y() + size_/2.0);
        path.lineTo(point_.x() + size_/2.0,
                    point_.y() - size_/2.0);

    }
    setPath(path);
}

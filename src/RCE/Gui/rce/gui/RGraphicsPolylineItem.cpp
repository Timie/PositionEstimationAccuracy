#include "RGraphicsPolylineItem.h"

#include <QPainterPath>
#include <QPainterPathStroker>

rce::gui::RGraphicsPolylineItem::
RGraphicsPolylineItem(const QPolygonF &path,
                      const QPen &pen,
                      QGraphicsItem *parent):
    QGraphicsPathItem(parent),
    polyline_(path),
    penWidth_(pen.widthF())
{
    setBrush(QBrush(Qt::transparent));

    setPen(pen);

    regenerate();
}

QPainterPath
rce::gui::RGraphicsPolylineItem::
lineShape() const
{
    return lineShape_;
}

void
rce::gui::RGraphicsPolylineItem::
regenerate()
{
    QPainterPath path;
    if(polyline_.size() != 1)
        path.addPolygon(polyline_);
    else
    {
        path.addRect(polyline_.first().x() - 0.5f,
                     polyline_.first().y() - 0.5f,
                     1,1);
    }

    setPath(path);

    QPainterPathStroker stroker;
    stroker.setCapStyle(pen().capStyle());
    stroker.setDashOffset(pen().dashOffset());
    stroker.setDashPattern(pen().dashPattern());
    stroker.setJoinStyle(pen().joinStyle());
    stroker.setMiterLimit(pen().miterLimit());
    stroker.setWidth(pen().widthF());


    lineShape_ = stroker.createStroke(path);
}

void
rce::gui::RGraphicsPolylineItem::
setPenWidth(const double penWidth)
{
    if (penWidth >= 0)
    {
        penWidth_ = penWidth;
        QPen newPen = pen();

        newPen.setWidth(penWidth_);
        setPen(newPen);
    }

    regenerate();
}

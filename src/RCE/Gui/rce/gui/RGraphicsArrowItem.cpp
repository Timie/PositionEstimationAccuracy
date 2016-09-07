#include "RGraphicsArrowItem.h"

#include <QGraphicsPathItem>
#include <QBrush>
#include <QColor>
#include <QPen>
//#include <QDebug>

#include "rce/gui/RGuiUtils.h"
#include "rce/gui/RGraphicsTextShapeItem.h"

rce::gui::RGraphicsArrowItem::
RGraphicsArrowItem(const QLineF &line,
                   const QString &text,
                   const QFont &font,
                   double lineWidth,
                   QGraphicsItem *parent):
    QGraphicsItemGroup(parent),
    arrowItem_(new QGraphicsPathItem(this)),
    textItem_(new RGraphicsTextShapeItem(text,
                                         font,
                                         Qt::AlignHCenter | Qt::AlignBottom,
                                         QPointF(0,-6),
                                         this)),
    arrowheadSize_(5),
    line_(line),
    text_(text)
{
    arrowItem_->hide();
    arrowItem_->setBrush(QColor(Qt::transparent));
    QPen linePen = arrowItem_->pen();
    linePen.setColor(QColor(0,100,255, 200));
    linePen.setWidthF(lineWidth);
    linePen.setCosmetic(true);
    arrowItem_->setPen(linePen);
    textItem_->setBrush(QColor(0,100,255, 255));
    textItem_->setPen(QPen(Qt::transparent));
    textItem_->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    textItem_->hide();
    regenerate();
}

rce::gui::RGraphicsArrowItem::
~RGraphicsArrowItem()
{

}

void
rce::gui::RGraphicsArrowItem::
setFont(const QFont &font)
{
    textItem_->setFont(font);
}

void
rce::gui::RGraphicsArrowItem::
setLinePen(const QPen &linePen)
{
    arrowItem_->setPen(linePen);
}

void
rce::gui::RGraphicsArrowItem::
setLineWidth(double value)
{
    QPen linePen = arrowItem_->pen();
    linePen.setWidthF(value);
    arrowItem_->setPen(linePen);
}

QFont
rce::gui::RGraphicsArrowItem::
getFont() const
{
    return textItem_->getFont();
}

void
rce::gui::RGraphicsArrowItem::
regenerate()
{
    arrowItem_->hide();
    textItem_->hide();

    if(line_.p1() != line_.p2())
    {
        // prepare arrow item
        QPainterPath arrowPath;
        arrowPath.moveTo(line_.p1());
        arrowPath.lineTo(line_.p2());

        QPainterPath arrowHead;
        arrowHead.moveTo(-arrowheadSize_,
                         arrowheadSize_);
        arrowHead.lineTo(0,0);
        arrowHead.lineTo(-arrowheadSize_,
                         -arrowheadSize_);
        {
            QPainterPath arrowHead1;
            QTransform arrowHeadTransform;
            arrowHeadTransform.translate(line_.p2().x(),
                                         line_.p2().y());
            arrowHeadTransform.rotate(-line_.angle());
            arrowHead1 = arrowHeadTransform.map(arrowHead);
            arrowPath.addPath(arrowHead1);
        }

        {
            QPainterPath arrowHead2;
            QTransform arrowHeadTransform;
            arrowHeadTransform.translate(line_.p1().x(),
                                         line_.p1().y());
            arrowHeadTransform.rotate(-line_.angle() + 180);
            arrowHead2 = arrowHeadTransform.map(arrowHead);
            arrowPath.addPath(arrowHead2);
        }

        arrowItem_->setPath(arrowPath);
        arrowItem_->show();

        // prepare text item
        if(!text_.isEmpty())
        {
            textItem_->show();
            textItem_->setText(text_);
            textItem_->setPos(line_.pointAt(0.5));
            textItem_->regenerate();
            //qDebug() << "textItme" << textItem_->path() << textItem_->shape();
        }
    }
}

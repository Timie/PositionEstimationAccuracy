#include "RGraphicsFlagItem.h"

#include <QGraphicsPathItem>
#include <QPen>
#include <QBrush>

#include "rce/gui/RGraphicsTextShapeItem.h"
#include "rce/gui/RGuiUtils.h"
#include "dfs/core/DDebug.h"

#define RCE_DEFAULT_FLAG_POLE_WIDTH (1.5)

rce::gui::RGraphicsFlagItem::
RGraphicsFlagItem(const QPointF &position,
                  const QString &flagText,
                  const QColor &textColour,
                  const QFont &font,
                  const QColor &flagColour,
                  QGraphicsItem *parent):
    QGraphicsItemGroup(parent)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setPos(position);

    labelItem_ = new rce::gui::RGraphicsTextShapeItem(flagText,
                                                      font,
                                                      Qt::AlignLeft | Qt::AlignBottom,
                                                      QPointF(),
                                                      NULL);

    labelItem_->setOriginShift(QPointF(RCE_FLAG_DSPLCMNT_X,
                                       -RCE_FLAG_DSPLCMNT_Y));
    addToGroup(labelItem_);
    labelItem_->setPen(Qt::NoPen);
    labelItem_->setBrush(QBrush(textColour));
    labelItem_->setPos(0,0);

    flagItem_ = new QGraphicsPathItem(NULL);
    addToGroup(flagItem_);
    flagItem_->setBrush(QBrush(flagColour));
    flagItem_->setPen(QPen(QBrush(flagColour),
                           RCE_DEFAULT_FLAG_POLE_WIDTH));
    flagItem_->setPos(0,0);

    flagItem_->setZValue(3);
    labelItem_->setZValue(4);

    //regenerate();
}

void
rce::gui::RGraphicsFlagItem::
setFlagText(const QString &text)
{
    labelItem_->setText(text);

    // regenerate();
}

void
rce::gui::RGraphicsFlagItem::
setFlagDisplacement(const QPointF &displacement)
{
    labelItem_->setOriginShift(displacement);
    // regenerate
}

void
rce::gui::RGraphicsFlagItem::
setFlagPoleWidth(qreal value)
{
    flagItem_->setPen(QPen(flagItem_->pen().brush().color(),
                           value));
}

void
rce::gui::RGraphicsFlagItem::
setTextColour(const QColor &colour)
{
    labelItem_->setBrush(QBrush(colour));
}

void
rce::gui::RGraphicsFlagItem::
setFont(const QFont &font)
{
    labelItem_->setFont(font);
}

void
rce::gui::RGraphicsFlagItem::
setFlagColour(const QColor &colour)
{
    QPen pen = flagItem_->pen();
    pen.setColor(colour);
    flagItem_->setPen(pen);
    flagItem_->setBrush(QBrush(colour));
}

QFont
rce::gui::RGraphicsFlagItem::
getFont() const
{
    return labelItem_->getFont();
}


QPainterPath
rce::gui::RGraphicsFlagItem::
getFlagPath() const
{
    return flagItem_->path();
}

QRectF
rce::gui::RGraphicsFlagItem::
boundingRect() const
{
    return flagItem_->mapToParent(flagItem_->boundingRect()).boundingRect();
}

QPainterPath
rce::gui::RGraphicsFlagItem::
shape() const
{
    return flagItem_->mapToParent(flagItem_->shape());
}

void
rce::gui::RGraphicsFlagItem::
regenerate()
{
    labelItem_->regenerate();

    flagItem_->setPath(generateFlagPath(labelItem_->boundingRect()));

    flagItem_->setPos(labelItem_->pos());
}

QPainterPath
rce::gui::RGraphicsFlagItem::
generateFlagPath(const QRectF &flagArea)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.moveTo(0,0);
    if((flagArea.width() > 0) &&
       (flagArea.height() > 0))
    {
        QRectF adjustedArea = flagArea.adjusted(-RCE_EMPHASED_LINE_WIDTH,
                                                -RCE_EMPHASED_LINE_WIDTH,
                                                RCE_EMPHASED_LINE_WIDTH,
                                                RCE_EMPHASED_LINE_WIDTH); // TODO: Make this automaticly scale with font size

        path.lineTo(adjustedArea.bottomLeft());
        path.addRect(adjustedArea);
    }

    path.moveTo(0,0);
    path.addEllipse(QPointF(0,0),
                    1.0, 1.0);

    return path;
}

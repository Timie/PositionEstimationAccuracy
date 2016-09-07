#include "RGraphicsTextShapeItem.h"


#include <QPainter>
#include <QFontMetricsF>

//rce::gui::RGraphicsTextShapeItem::
//RGraphicsTextShapeItem(const QPainterPath &path,
//                       QGraphicsItem *parent):
//    QGraphicsPathItem(path, parent)
//{

//    lineSize_ = QFontMetricsF(font_).lineSpacing();
//}



rce::gui::RGraphicsTextShapeItem::
RGraphicsTextShapeItem(const QString &text,
                       const QFont &font,
                       Qt::Alignment originAlignment,
                       const QPointF &originShift,
                       QGraphicsItem *parent):
    QGraphicsPathItem(parent),
    text_(text),
    font_(font),
    originAlignment_(originAlignment),
    originShift_(originShift)
{
    lineSize_ = QFontMetricsF(font_).lineSpacing();
    fontAscent_ = QFontMetrics(font_).ascent();
    regenerate();
}




void
rce::gui::RGraphicsTextShapeItem::
setText(const QString &text)
{
    text_ = text;
    //regenerate();
}

void
rce::gui::RGraphicsTextShapeItem::
setFont(const QFont &font)
{
    font_ = font;
    lineSize_ = QFontMetricsF(font_).lineSpacing();
    fontAscent_ = QFontMetrics(font_).ascent();
    //regenerate();
}

void
rce::gui::RGraphicsTextShapeItem::
setOriginShift(const QPointF &pos)
{
    originShift_ = pos;
    //regenerate();
}

void
rce::gui::RGraphicsTextShapeItem::
setOriginAlignment(Qt::Alignment alignment)
{
    originAlignment_ = alignment;
}


void
rce::gui::RGraphicsTextShapeItem::paint(QPainter *painter,
                                        const QStyleOptionGraphicsItem *option,
                                        QWidget *widget)
{
    bool aaWasOn = painter->renderHints().testFlag(QPainter::Antialiasing);
    if(!aaWasOn)
    {
        painter->setRenderHint(QPainter::Antialiasing,
                               true);
    }

    QGraphicsPathItem::paint(painter, option, widget);
    if(!aaWasOn)
    {
        painter->setRenderHint(QPainter::Antialiasing,
                               false);
    }
}

void
rce::gui::RGraphicsTextShapeItem::
regenerate()
{
    if(text_.isEmpty())
    {
        setPath(QPainterPath());
    }
    else
    {
        QPainterPath textPath;
        QStringList list = text_.split(QRegExp("\n|\r\n|\r"));

        for(int i = 0;
            i < list.size();
            ++i)
        {
            textPath.addText(0, i * lineSize_,
                             font_, list[i]);
        }

        textPath.translate(-(textPath.boundingRect().topLeft()));

        QPointF alignmentShift(0,0);
        if(originAlignment_.testFlag(Qt::AlignRight))
        {
            alignmentShift.setX(-textPath.boundingRect().width());
        }
        else if(originAlignment_.testFlag(Qt::AlignHCenter))
        {
            alignmentShift.setX(-textPath.boundingRect().width() / 2.0);
        }

        if(originAlignment_.testFlag(Qt::AlignBottom))
        {
            alignmentShift.setY(-textPath.boundingRect().height());
        }
        else if(originAlignment_.testFlag(Qt::AlignVCenter))
        {
            alignmentShift.setY(-textPath.boundingRect().height() / 2.0);
        }

        textPath.translate(originShift_.x() + alignmentShift.x(),
                           originShift_.y() + alignmentShift.y() /*+ fontAscent_*/);

        setPath(textPath);
    }
}

//QRectF rce::gui::RGraphicsTextShapeItem::boundingRect() const
//{

//}

//QPainterPath rce::gui::RGraphicsTextShapeItem::shape() const
//{

//}

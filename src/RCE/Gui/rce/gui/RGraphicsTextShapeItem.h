#ifndef RCE_GUI_RGRAPHICSTEXTSHAPEITEM_H
#define RCE_GUI_RGRAPHICSTEXTSHAPEITEM_H

#include <QGraphicsPathItem>
#include <QString>
#include <QFont>
#include <QPointF>

namespace rce {
    namespace gui {

        class RGraphicsTextShapeItem : public QGraphicsPathItem
        {
        public:

//            RGraphicsTextShapeItem(const QPainterPath &path,
//                                   QGraphicsItem *parent = NULL);
            RGraphicsTextShapeItem(const QString &text,
                                   const QFont &font,
                                   Qt::Alignment originAlignment,
                                   const QPointF &originShift = QPointF(),
                                   QGraphicsItem *parent = NULL);

            void setText(const QString &text);
            const QString &getText() const {return text_;}
            void setFont(const QFont &font);
            const QFont &getFont() const {return font_;}
            void setOriginShift(const QPointF &pos);
            void setOriginAlignment(Qt::Alignment alignment);


//            virtual QRectF boundingRect() const;
//            virtual QPainterPath shape() const;


            void regenerate();

        protected:
            virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget);



        protected:
            QString text_;
            QFont font_;
            Qt::Alignment originAlignment_;
            QPointF originShift_;

            double lineSize_;
            double fontAscent_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RGRAPHICSTEXTSHAPEITEM_H

#ifndef RCE_GUI_RGRAPHICSARROWITEM_H
#define RCE_GUI_RGRAPHICSARROWITEM_H

#include <QGraphicsItemGroup>
#include <QLineF>
#include <QString>

namespace rce {
    namespace gui {
        class RGraphicsTextShapeItem;

        class RGraphicsArrowItem : public QGraphicsItemGroup
        {
        public:
            RGraphicsArrowItem(const QLineF &line,
                               const QString &text,
                               const QFont &font, // should be bold and true
                               double lineWidth,
                               QGraphicsItem *parent = nullptr);
            virtual ~RGraphicsArrowItem();

            void setLine(const QLineF &line) {line_ = line;}
            void setText(const QString &text) {text_ = text;}
            void setFont(const QFont &font);
            void setLinePen(const QPen& linePen);
            void setLineWidth(double value);
            void setArrowHeadSize(double arrowheadSize) {arrowheadSize_ = arrowheadSize;}

            QFont getFont() const;

            const QLineF &getLine() const {return line_;}


            void regenerate();
        private:
            // static QFont getDefaultFont();

        private:
            QGraphicsPathItem *arrowItem_;
            RGraphicsTextShapeItem *textItem_;

            double arrowheadSize_;
            QLineF line_;
            QString text_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RGRAPHICSARROWITEM_H

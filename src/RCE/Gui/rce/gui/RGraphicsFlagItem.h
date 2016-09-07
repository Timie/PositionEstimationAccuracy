#ifndef RCE_GUI_RGRAPHICSFLAGITEM_H
#define RCE_GUI_RGRAPHICSFLAGITEM_H

#include <QGraphicsItemGroup>
#include <QString>
#include <QPointF>
#include <QFont>

class QGraphicsPathItem;

namespace rce {
    namespace gui {

        class RGraphicsTextShapeItem;

        // based upon implementation in RGraphicsTrackingHistoryItem
        class RGraphicsFlagItem : public QGraphicsItemGroup
        {
        public:
            RGraphicsFlagItem(const QPointF &position = QPointF(0,0),
                              const QString &flagText = QString(),
                              const QColor &textColour = QColor(Qt::white),
                              const QFont &font = QFont(),
                              const QColor &flagColour = QColor(Qt::red),
                              QGraphicsItem *parent = NULL);

            void setFlagText(const QString &text); // empty text = no flag, whitespace text = empty flag
            void setFlagDisplacement(const QPointF &displacement);
            void setFlagPoleWidth(qreal value);
            void setTextColour(const QColor &colour);
            void setFont(const QFont &font);
            void setFlagColour(const QColor &colour);
            //void setPosition(const QPointF &position); // replace by "setPos"

            QFont getFont() const;

            QPainterPath getFlagPath() const;

            QRectF boundingRect() const;
            QPainterPath shape() const;


            void regenerate();

        protected:
            QPainterPath generateFlagPath(const QRectF &flagArea);


        protected:
            // items
            RGraphicsTextShapeItem *labelItem_;
            QGraphicsPathItem *flagItem_;

            // data are all stored in items

        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RGRAPHICSFLAGITEM_H

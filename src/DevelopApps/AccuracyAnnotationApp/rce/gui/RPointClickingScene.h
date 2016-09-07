#ifndef RCE_GUI_RPOINTCLICKINGSCENE_H
#define RCE_GUI_RPOINTCLICKINGSCENE_H

#include <QMap>

#include "rce/gui/RImageWidgetScene.h"

namespace rce {
    namespace gui {
        class RGraphicsFlagItem;

        class RPointClickingScene : public rce::gui::RImageWidgetScene
        {
            Q_OBJECT
        public:
            RPointClickingScene(QObject *parent = NULL);

        public slots:
            void addFlag(qint64 id,
                         const QPointF &scenePos,
                         const QString &text);
            void removeFlag(qint64 id);
            void clear();

        signals:
            void clicked(QPointF scenePos);
            void flagRemoveRequested(qint64 id);



        protected:
            virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
            virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
        private: // attributes
            QMap<qint64, rce::gui::RGraphicsFlagItem*> labeledPointItems_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RPOINTCLICKINGSCENE_H

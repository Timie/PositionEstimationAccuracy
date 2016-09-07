#include "RPointClickingScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QGraphicsView>


#include "rce/gui/RGraphicsFlagItem.h"


rce::gui::RPointClickingScene::
RPointClickingScene(QObject *parent):
    RImageWidgetScene(parent)
{

}

void
rce::gui::RPointClickingScene::
addFlag(qint64 id,
        const QPointF &scenePos,
        const QString &text)
{
    auto it = labeledPointItems_.find(id);
    if(it != labeledPointItems_.end())
    {
        it.value()->setPos(scenePos);
        it.value()->setFlagText(text);
        it.value()->regenerate();
    }
    else
    {
        rce::gui::RGraphicsFlagItem *newFlagItem = new rce::gui::RGraphicsFlagItem(scenePos,
                                                                                   text);
        addItem(newFlagItem);
        newFlagItem->regenerate();

        labeledPointItems_.insert(id,
                                  newFlagItem);
    }

}

void
rce::gui::RPointClickingScene::
removeFlag(qint64 id)
{
    auto it = labeledPointItems_.find(id);
    if(it != labeledPointItems_.end())
    {
        removeItem(it.value());
        labeledPointItems_.erase(it);
    }
}

void
rce::gui::RPointClickingScene::
clear()
{
    rce::gui::RImageWidgetScene::clear();
    labeledPointItems_.clear();
}

void
rce::gui::RPointClickingScene::
mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit clicked(event->scenePos());
    }

    RImageWidgetScene::mouseReleaseEvent(event);
}

void rce::gui::RPointClickingScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    const QGraphicsView *eventView = NULL;
    if(event->widget() != NULL)
    {
        eventView = dynamic_cast<const QGraphicsView*>(event->widget()->parentWidget());
    }

    QTransform viewTransform;
    if(eventView != NULL)
    {
        viewTransform = eventView->transform();
    }

    QList<QGraphicsItem *> itemsUnderPos = items(event->scenePos(),
                                                 Qt::IntersectsItemBoundingRect,
                                                 Qt::DescendingOrder,
                                                 viewTransform);

    for(int i = 0;
        i < itemsUnderPos.size();
        ++i)
    {
        rce::gui::RGraphicsFlagItem *item = dynamic_cast<rce::gui::RGraphicsFlagItem *>(itemsUnderPos[i]);
        if(item != nullptr)
        {
            for(auto it = labeledPointItems_.begin();
                it != labeledPointItems_.end();
                ++it)
            {
                if(it.value() == item)
                {
                    QMenu *ctxMenu = new QMenu(NULL);

                    QAction *removeAction = ctxMenu->addAction(tr("Remove point %1").arg(it.key()));

                    QAction *selectedAction = ctxMenu->exec(event->screenPos());
                    if(selectedAction == removeAction)
                    {
                        emit flagRemoveRequested(it.key());
                    }

                    ctxMenu->deleteLater();
                    event->accept();
                    return;

                }
            }
        }
    }

    RImageWidgetScene::contextMenuEvent(event);
}

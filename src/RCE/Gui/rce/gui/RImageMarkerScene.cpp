#include "RImageMarkerScene.h"

//#include <QDebug>

#include <QPainter>
#include <QVariant>
#include <QMenu>
#include <QGraphicsPolygonItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsRectItem>
#include <QGraphicsView>

#include "rce/gui/RGraphicsPolylineItem.h"
#include "rce/gui/RGraphicsCrossItem.h"
#include "rce/gui/RGuiUtils.h"



// half of the size of the handle points
#define RCE_HANDLE_SIZE (4)

#define RCE_ITEM_TYPE_DATA_INDEX (0)
#define RCE_ITEM_ID_DATA_INDEX (1)

rce::gui::RImageMarkerScene::
RImageMarkerScene(QObject *parent):
    rce::gui::RImageWidgetScene(
                   parent),
    mode_(PickAnyMode),
    drawnItem_(NULL),
    pickedItem_(NULL)
{

}

rce::gui::RImageMarkerScene::
~RImageMarkerScene()
{

}

QPolygonF
rce::gui::RImageMarkerScene::
getPolygonShape(quint32 id)
{
    auto it = polygonItems_.find(id);
    if(it != polygonItems_.end())
    {
        QGraphicsPolygonItem *item = it.value();
        QPolygonF polygon = item->polygon();
        openPolygon(polygon);

        return polygon;
    }
    else
    {
        return QPolygonF();
    }
}

QPolygonF
rce::gui::RImageMarkerScene::
getPolylineShape(quint32 id)
{
    auto it = polylineItems_.find(id);
    if(it != polylineItems_.end())
    {
        rce::gui::RGraphicsPolylineItem *item = it.value();
        return item->getPolyline();
    }
    else
    {
        return QPolygonF();
    }
}

QPointF
rce::gui::RImageMarkerScene::
getPoint(quint32 id)
{
    auto it = pointItems_.find(id);
    if(it != pointItems_.end())
    {
        rce::gui::RGraphicsCrossItem *item = it.value();
        return item->getPoint();
    }
    else
    {
        return QPointF();
    }
}

void
rce::gui::RImageMarkerScene::
addPolygonObject(const QPolygonF &polygon,
                  const QBrush &brush,
                  const QPen &pen,
                  quint32 id)
{

    // remove old if exists
    deletePolygon(id);

    // create new
    QPolygonF closedPolygon = polygon;
    closePolygon(closedPolygon);

    QGraphicsPolygonItem *newItem = addPolygon(closedPolygon, pen, brush);
    newItem->setData(RCE_ITEM_TYPE_DATA_INDEX, PolygonType);
    newItem->setData(RCE_ITEM_ID_DATA_INDEX, id);
    newItem->setZValue(1);

    polygonItems_[id] = newItem;
}

void
rce::gui::RImageMarkerScene::
addPolylineObject(const QPolygonF &polyline,
                  const QPen &pen,
                  quint32 id)
{
    deletePolyline(id);

    rce::gui::RGraphicsPolylineItem *newItem = new rce::gui::RGraphicsPolylineItem(polyline,
                                                                                   pen,
                                                                                   NULL);
    addItem(newItem);

    newItem->setData(RCE_ITEM_TYPE_DATA_INDEX, PolylineType);
    newItem->setData(RCE_ITEM_ID_DATA_INDEX, id);
    newItem->setZValue(2);

    polylineItems_[id] = newItem;
}

void
rce::gui::RImageMarkerScene::
addPointObject(const QPointF &point,
         const QPen &pen,
         double size,
         quint32 id)
{
    deletePoint(id);

    rce::gui::RGraphicsCrossItem *newItem = new rce::gui::RGraphicsCrossItem(point,
                                                                             pen,
                                                                             size,
                                                                             NULL);
    addItem(newItem);

    newItem->setData(RCE_ITEM_TYPE_DATA_INDEX, PointType);
    newItem->setData(RCE_ITEM_ID_DATA_INDEX, id);
    newItem->setZValue(3);

    pointItems_[id] = newItem;
}

void
rce::gui::RImageMarkerScene::
setPolylinePen(quint32 id,
               const QPen &pen,
               double penWidth)
{
    auto it = polylineItems_.find(id);
    if(it != polylineItems_.end())
    {
        rce::gui::RGraphicsPolylineItem *item = it.value();

        item->setPen(pen);
        item->setPenWidth(penWidth);

    }
}

void
rce::gui::RImageMarkerScene::
setPolygonBrushAndPen(quint32 id,
                      const QBrush &brush,
                      const QPen &pen)
{
    auto it = polygonItems_.find(id);
    if(it != polygonItems_.end())
    {
        QGraphicsPolygonItem *polygonItem = it.value();
        polygonItem->setBrush(brush);
        polygonItem->setPen(pen);
    }
}

void
rce::gui::RImageMarkerScene::
setPointPen(quint32 id,
            const QPen &pen)
{
    auto it = pointItems_.find(id);
    if(it != pointItems_.end())
    {
        rce::gui::RGraphicsCrossItem *item = it.value();
        item->setPen(pen);
    }
}

void
rce::gui::RImageMarkerScene::
setPointSize(quint32 id,
             double size)
{
    auto it = pointItems_.find(id);
    if(it != pointItems_.end())
    {
        rce::gui::RGraphicsCrossItem *item = it.value();
        item->setSize(size);
        item->regenerate();
    }
}

void
rce::gui::RImageMarkerScene::
editPolygon(quint32 id)
{
    auto it = polygonItems_.find(id);
    if(it != polygonItems_.end())
    {
        finishCurrentMode();
        mode_ = DrawPolygonMode;

        QGraphicsPolygonItem *polygonItem = it.value();
        drawnItem_ = polygonItem;

        QPolygonF polygon = polygonItem->polygon();
        openPolygon(polygon);
        drawnPolyobject_ = polygon;
        drawnItemID_ = id;


        addManipulationHandles();
    }
}

void
rce::gui::RImageMarkerScene::
editPolyline(quint32 id)
{
    auto it = polylineItems_.find(id);
    if(it != polylineItems_.end())
    {
        finishCurrentMode();
        mode_ = DrawPolylineMode;

        rce::gui::RGraphicsPolylineItem *polylineItem = it.value();
        drawnItem_ = polylineItem;
        drawnItemID_ = id;

        drawnPolyobject_ = polylineItem->getPolyline();

        addManipulationHandles();
    }
}

void
rce::gui::RImageMarkerScene::
editPoint(quint32 id)
{
    auto it = pointItems_.find(id);
    if(it != pointItems_.end())
    {
        finishCurrentMode();
        mode_ = DrawPointMode;

        rce::gui::RGraphicsCrossItem *item = it.value();
        drawnItem_ = item;
        drawnItemID_ = id;

        drawnPolyobject_.clear();
        drawnPolyobject_.push_back(item->getPoint());

        addManipulationHandles();
    }
}

void
rce::gui::RImageMarkerScene::
deleteObject(quint32 id)
{


    deletePolygon(id);
    deletePolyline(id);
    deletePoint(id);

}

void
rce::gui::RImageMarkerScene::
deletePolygon(quint32 id)
{
    if(pickedItem_ != NULL)
    {
        if(pickedItem_->data(RCE_ITEM_ID_DATA_INDEX).toUInt() == id)
        {
            pickedItem_ = NULL;
        }
    }

    auto it = polygonItems_.find(id);
    if(it != polygonItems_.end())
    {
        removeItem(it.value());
        delete it.value();
        polygonItems_.erase(it);
    }
}

void
rce::gui::RImageMarkerScene::
deletePolyline(quint32 id)
{
    if(pickedItem_ != NULL)
    {
        if(pickedItem_->data(RCE_ITEM_ID_DATA_INDEX).toUInt() == id)
        {
            pickedItem_ = NULL;
        }
    }

    auto it = polylineItems_.find(id);
    if(it != polylineItems_.end())
    {
        removeItem(it.value());
        delete it.value();
        polylineItems_.erase(it);
    }
}

void
rce::gui::RImageMarkerScene::
deletePoint(quint32 id)
{
    if(pickedItem_ != NULL)
    {
        if(pickedItem_->data(RCE_ITEM_ID_DATA_INDEX).toUInt() == id)
        {
            pickedItem_ = NULL;
        }
    }

    auto it = pointItems_.find(id);
    if(it != pointItems_.end())
    {
        removeItem(it.value());
        delete it.value();
        pointItems_.erase(it);
    }
}

void
rce::gui::RImageMarkerScene::
setMode(rce::gui::RImageMarkerScene::RMode mode)
{
    finishCurrentMode();
    mode_ = mode;
}

void
rce::gui::RImageMarkerScene::
setAllPolylinesVisible(bool value)
{

    for(auto it = polylineItems_.begin();
        it != polylineItems_.end();
        ++it)
    {
        rce::gui::RGraphicsPolylineItem *item = it.value();
        item->setVisible(value);
    }
}

void
rce::gui::RImageMarkerScene::
setAllPolygonsVisible(bool value)
{
    for(auto it = polygonItems_.begin();
        it != polygonItems_.end();
        ++it)
    {
        QGraphicsPolygonItem *item = it.value();
        item->setVisible(value);
    }
}

void
rce::gui::RImageMarkerScene::
setAllPointsVisible(bool value)
{
    for(auto it = pointItems_.begin();
        it != pointItems_.end();
        ++it)
    {
        rce::gui::RGraphicsCrossItem *item = it.value();
        item->setVisible(value);
    }
}

void
rce::gui::RImageMarkerScene::
setPolygonVisble(quint32 id,
                 bool value)
{
    auto it = polygonItems_.find(id);
    if(it != polygonItems_.end())
    {
        it.value()->setVisible(value);
    }
}

void
rce::gui::RImageMarkerScene::
setPolylineVisible(quint32 id,
                   bool value)
{
    auto it = polylineItems_.find(id);
    if(it != polylineItems_.end())
    {
        it.value()->setVisible(value);
    }
}

void
rce::gui::RImageMarkerScene::
setPointVisible(quint32 id,
                bool value)
{
    auto it = pointItems_.find(id);
    if(it != pointItems_.end())
    {
        it.value()->setVisible(value);
    }
}

void
rce::gui::RImageMarkerScene::
clear()
{
    finishCurrentMode();
    mode_ = PickAnyMode;
    RImageWidgetScene::clear();
    drawnPolyobject_.clear();
    drawnItem_ = NULL;
    drawnItemID_ = 0;
    handles_.clear();
    activeHandleIdx_ = -1;
    itemEdited_ = false;
    pickedItem_ = NULL;
    polygonItems_.clear();
    polylineItems_.clear();
    pointItems_.clear();
}



void
rce::gui::RImageMarkerScene::
mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton)
    {
        const QGraphicsView *eventView = NULL;
        if(mouseEvent->widget() != NULL)
        {
            eventView = dynamic_cast<const QGraphicsView*>(mouseEvent->widget()->parentWidget());
        }

        QTransform viewTransform;
        if(eventView != NULL)
        {
            viewTransform = eventView->transform();
        }

        itemEdited_ = false;
        switch(mode_)
        {
            case rce::gui::RImageMarkerScene::DrawPointMode:
                {
                    // check if there is handle under mouse
                    QList<QGraphicsItem *> itemsUnderMouse = items(rce::gui::getSceneRectAroundScreenPos(mouseEvent->screenPos(),
                                                                                                         mouseEvent->scenePos(),
                                                                                                         dynamic_cast<const QGraphicsView*>(mouseEvent->widget()->parentWidget())),
                                                                   Qt::IntersectsItemBoundingRect,
                                                                   Qt::DescendingOrder,
                                                                   viewTransform);
                    QGraphicsItem *handle = NULL;
                    for(int i = 0; i < itemsUnderMouse.size(); ++i)
                    {
                        if((itemsUnderMouse[i]->data(RCE_ITEM_TYPE_DATA_INDEX).toUInt() == HandleType)
                           && itemsUnderMouse[i]->boundingRect().contains(itemsUnderMouse[i]->mapFromScene(mouseEvent->scenePos())))
                        {
                            handle = itemsUnderMouse[i];
                            break;
                        }
                    }

                    if(handle != NULL)
                    { // there is handle, set is as active
                        activeHandleIdx_ = 0;
                    }
                    else
                    {
                        activeHandleIdx_ = -1;
                    }
                }
                break;
            case rce::gui::RImageMarkerScene::DrawPolygonMode:
            case rce::gui::RImageMarkerScene::DrawPolylineMode:
                {
                    // check if there is handle under mouse
                    QList<QGraphicsItem *> itemsUnderMouse = items(rce::gui::getSceneRectAroundScreenPos(mouseEvent->screenPos(),
                                                                                                         mouseEvent->scenePos(),
                                                                                                         dynamic_cast<const QGraphicsView*>(mouseEvent->widget()->parentWidget())),
                                                                   Qt::IntersectsItemBoundingRect,
                                                                   Qt::DescendingOrder,
                                                                   viewTransform);
                    QGraphicsItem *handle = NULL;
                    for(int i = 0; i < itemsUnderMouse.size(); ++i)
                    {
                        if((itemsUnderMouse[i]->data(RCE_ITEM_TYPE_DATA_INDEX).toUInt() == HandleType)
                           && itemsUnderMouse[i]->boundingRect().contains(itemsUnderMouse[i]->mapFromScene(mouseEvent->scenePos())))
                        {
                            handle = itemsUnderMouse[i];
                            break;
                        }
                    }

                    if(handle != NULL)
                    { // there is handle, set is as active
                        activeHandleIdx_ = handle->data(RCE_ITEM_ID_DATA_INDEX).toInt();
                    }
                    else
                    { // there is not handle - create new
                        addControlPoint(mouseEvent->scenePos());
                        itemEdited_ = true;
                        activeHandleIdx_ = handles_.size() - 1;
                    }
                }
                break;
            case rce::gui::RImageMarkerScene::PickPolygonMode:
            case rce::gui::RImageMarkerScene::PickPolylineMode:
            case rce::gui::RImageMarkerScene::PickPointMode:
            case rce::gui::RImageMarkerScene::PickAnyMode:
                {
                    QVector<quint32> polygons;
                    QVector<quint32> polylines;
                    QVector<quint32> points;

                    getItemsAtPosition(rce::gui::getSceneRectAroundScreenPos(mouseEvent->screenPos(),
                                                                             mouseEvent->scenePos(),
                                                                             dynamic_cast<const QGraphicsView*>(mouseEvent->widget()->parentWidget())),
                                       polygons,
                                       polylines,
                                       points,
                                       viewTransform);

                    if(polygons.size() + polylines.size() + points.size() > 1)
                    { // show menu

                        QMap<QAction *, quint32> actionToPolygon;
                        QMap<QAction *, quint32> actionToPolyline;
                        QMap<QAction *, quint32> actionToPoint;


                        QMenu *ctxMenu = new QMenu(mouseEvent->widget());
                        ctxMenu->setTitle(tr("Pick Item..."));

                        foreach(quint32 polygonID,
                                polygons)
                        {
                            actionToPolygon[ctxMenu->addAction(getPolygonDisplayName(polygonID))] = polygonID;
                        }
                        ctxMenu->addSeparator();

                        foreach(quint32 polylineID,
                                polylines)
                        {
                            actionToPolyline[ctxMenu->addAction(getPolylineDisplayName(polylineID))] = polylineID;
                        }
                        ctxMenu->addSeparator();

                        foreach(quint32 pointID,
                                points)
                        {
                            actionToPoint[ctxMenu->addAction(getPointDisplayName(pointID))] = pointID;
                        }


                        QAction *selectedAction = ctxMenu->exec(mouseEvent->screenPos());

                        if(actionToPolygon.contains(selectedAction))
                        {
                            pickedItem_ = polygonItems_[actionToPolygon[selectedAction]];
                            emit pickedPolygon(actionToPolygon[selectedAction]);
                        }
                        else if(actionToPolyline.contains(selectedAction))
                        {
                            pickedItem_ = polylineItems_[actionToPolyline[selectedAction]];
                            emit pickedPolyline(actionToPolyline[selectedAction]);
                        }
                        else if(actionToPoint.contains(selectedAction))
                        {
                            pickedItem_ = pointItems_[actionToPoint[selectedAction]];
                            emit pickedPoint(actionToPoint[selectedAction]);
                        }
                        else
                        {
                            pickedItem_ = NULL;
                            emit pickedNothing();
                        }

                        ctxMenu->deleteLater();
                    }
                    else
                    {
                        if(polygons.size() == 1)
                        {
                            pickedItem_ = polygonItems_[polygons[0]];
                            emit pickedPolygon(polygons[0]);
                        }
                        else if(polylines.size() == 1)
                        {
                            pickedItem_ = polylineItems_[polylines[0]];
                            emit pickedPolyline(polylines[0]);
                        }
                        else if(points.size() == 1)
                        {
                            pickedItem_ = pointItems_[points[0]];
                            emit pickedPoint(points[0]);
                        }
                        else
                        {
                            pickedItem_ = NULL;
                            emit pickedNothing();
                        }
                    }
                }
                break;
        }
    }
    else
        RImageWidgetScene::mousePressEvent(mouseEvent);
}

void
rce::gui::RImageMarkerScene::
mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->buttons().testFlag(Qt::LeftButton))
    {
        //bool moved = false;
        switch(mode_)
        {
            case rce::gui::RImageMarkerScene::DrawPointMode:
            case rce::gui::RImageMarkerScene::DrawPolygonMode:
            case rce::gui::RImageMarkerScene::DrawPolylineMode:
                {
                    if(activeHandleIdx_ >= 0)
                    {
                        // move handle and edit underlining polyobject
                        movePoint(mouseEvent->scenePos());
                        itemEdited_ = true;
                        //moved = true;
                    }
                }
                break;
            case rce::gui::RImageMarkerScene::PickPointMode:
            case rce::gui::RImageMarkerScene::PickPolygonMode:
            case rce::gui::RImageMarkerScene::PickPolylineMode:
            case rce::gui::RImageMarkerScene::PickAnyMode:
                break;

        }
//        if(!moved)
//        {
//            RImageWidgetScene::mouseMoveEvent(mouseEvent);
//        }
    }
    else
        RImageWidgetScene::mouseMoveEvent(mouseEvent);
}

void
rce::gui::RImageMarkerScene::
mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton)
    {
        switch(mode_)
        {
            case rce::gui::RImageMarkerScene::DrawPointMode:
            case rce::gui::RImageMarkerScene::DrawPolygonMode:
            case rce::gui::RImageMarkerScene::DrawPolylineMode:
                {
                    activeHandleIdx_ = -1;

                    if(itemEdited_)
                    {
                        itemEdited_ = false;
                        if(mode_ == rce::gui::RImageMarkerScene::DrawPointMode)
                        {
                            emit editedPoint(drawnItemID_);
                        }
                        else if(mode_ == rce::gui::RImageMarkerScene::DrawPolygonMode)
                        {
                            emit editedPolygon(drawnItemID_);
                        }
                        else
                        {
                            emit editedPolyline(drawnItemID_);
                        }
                    }
                }
                break;
            case rce::gui::RImageMarkerScene::PickPolygonMode:
            case rce::gui::RImageMarkerScene::PickPolylineMode:
            case rce::gui::RImageMarkerScene::PickPointMode:
            case rce::gui::RImageMarkerScene::PickAnyMode:
            default:
                break;

        }
    }
    else
        RImageWidgetScene::mouseReleaseEvent(mouseEvent);
}

void
rce::gui::RImageMarkerScene::
contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if((mode_ == DrawPolylineMode) ||
       (mode_ == DrawPointMode) ||
       (mode_ == DrawPolygonMode))
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

        // check if there is handle under mouse
        QList<QGraphicsItem *> itemsUnderMouse = items(rce::gui::getSceneRectAroundScreenPos(event->screenPos(),
                                                                                             event->scenePos(),
                                                                                             dynamic_cast<const QGraphicsView*>(event->widget()->parentWidget())),
                                                       Qt::IntersectsItemBoundingRect,
                                                       Qt::DescendingOrder,
                                                       viewTransform);
        QGraphicsItem *handle = NULL;
        for(int i = 0; i < itemsUnderMouse.size(); ++i)
        {
            if((itemsUnderMouse[i]->data(RCE_ITEM_TYPE_DATA_INDEX).toUInt() == HandleType)
               && itemsUnderMouse[i]->boundingRect().contains(itemsUnderMouse[i]->mapFromScene(event->scenePos())))
            {
                handle = itemsUnderMouse[i];
                break;
            }
        }

        if((handle != NULL) && (drawnPolyobject_.size() > 1))
        { // there is handle, set is as active
            activeHandleIdx_ = handle->data(RCE_ITEM_ID_DATA_INDEX).toInt();


            QMenu *ctxMenu = new QMenu(event->widget());
            QAction *removeControlPointAction = ctxMenu->addAction(tr("Remove Control Point"));

            QAction *selectedAction = ctxMenu->exec(event->screenPos());

            if(selectedAction == removeControlPointAction)
            {
                removePoint(activeHandleIdx_);
            }

            ctxMenu->deleteLater();
        }
    }
    else
    {
        RImageWidgetScene::contextMenuEvent(event);
    }
}

void
rce::gui::RImageMarkerScene::
finishCurrentMode()
{
    drawnPolyobject_.clear();
    drawnItem_ = NULL;
    removeManipulationHandles();
    activeHandleIdx_ = -1;
    /*switch(mode_)
    {
        case rce::gui::RImageMarkerScene::DrawPolygon:
            break;
        case rce::gui::RImageMarkerScene::PickPolygon:
            break;
        case rce::gui::RImageMarkerScene::DrawPolyline:
            break;
        case rce::gui::RImageMarkerScene::PickPolyline:
            break;
        case rce::gui::RImageMarkerScene::PickAny:
            break;

    }*/
}

void
rce::gui::RImageMarkerScene::
addManipulationHandles()
{
    removeManipulationHandles();
    for(int i = 0; i < drawnPolyobject_.size(); ++i)
    {
        addHandle(drawnPolyobject_[i]);
    }
}

void
rce::gui::RImageMarkerScene::
removeManipulationHandles()
{
    for(int i = 0; i < handles_.size(); ++i)
    {
        removeItem(handles_[i]);
        delete handles_[i];
    }
    handles_.clear();
}

void
rce::gui::RImageMarkerScene::
addControlPoint(const QPointF &position)
{
    if(mode_ == DrawPolygonMode)
    {
        addHandle(position);
        drawnPolyobject_.push_back(position);

        QGraphicsPolygonItem *polygonItem = static_cast<QGraphicsPolygonItem *>(drawnItem_);
        QPolygonF polygon = drawnPolyobject_;
        // make sure it is closed
        closePolygon(polygon);

        polygonItem->setPolygon(polygon);
    }
    else if(mode_ == DrawPolylineMode)
    {
        addHandle(position);
        drawnPolyobject_.push_back(position);

        rce::gui::RGraphicsPolylineItem *polylineItem = static_cast<rce::gui::RGraphicsPolylineItem *>(drawnItem_);

        polylineItem->setPolyline(drawnPolyobject_);
        polylineItem->regenerate();
    }
    else if(mode_ == DrawPointMode)
    {
        // Nothing...
    }
}

void
rce::gui::RImageMarkerScene::
addHandle(const QPointF &position)
{
    QGraphicsRectItem *newHandle = addRect(0 - RCE_HANDLE_SIZE,
                                           0 - RCE_HANDLE_SIZE,
                                           RCE_HANDLE_SIZE * 2, RCE_HANDLE_SIZE * 2,
                                           QPen(),
                                           QBrush(Qt::black));
    newHandle->setZValue(0);
    newHandle->setPos(position);

    newHandle->setTransformOriginPoint(newHandle->transformOriginPoint().x() + RCE_HANDLE_SIZE,
                                       newHandle->transformOriginPoint().y() + RCE_HANDLE_SIZE);

    newHandle->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    newHandle->setData(RCE_ITEM_TYPE_DATA_INDEX, HandleType);
    newHandle->setData(RCE_ITEM_ID_DATA_INDEX, handles_.size());

    handles_.push_back(newHandle);
}

void
rce::gui::RImageMarkerScene::
movePoint(const QPointF &position)
{
    // first - move handle
    //qDebug() << "Handle position prior to move:" << handles_[activeHandleIdx_]->pos() << handles_[activeHandleIdx_]->scenePos();
    handles_[activeHandleIdx_]->setPos(position);
    //qDebug() << "Handle position after move:" << handles_[activeHandleIdx_]->pos() << handles_[activeHandleIdx_]->scenePos();

    // now edit point in polyobject
    drawnPolyobject_[activeHandleIdx_] = position;

    // ...and item...
    if(mode_ == DrawPolygonMode)
    {
        QGraphicsPolygonItem *polygonItem = static_cast<QGraphicsPolygonItem *>(drawnItem_);
        QPolygonF polygon = drawnPolyobject_;
        closePolygon(polygon);
        polygonItem->setPolygon(polygon);
    }
    else if(mode_ == DrawPolylineMode)
    {
        rce::gui::RGraphicsPolylineItem *polylineItem = dynamic_cast<rce::gui::RGraphicsPolylineItem *>(drawnItem_);

        polylineItem->setPolyline(drawnPolyobject_);
        polylineItem->regenerate();
    }
    else if(mode_ == DrawPointMode)
    {
        rce::gui::RGraphicsCrossItem *pointItem = static_cast<rce::gui::RGraphicsCrossItem *>(drawnItem_);

        pointItem->setPoint(drawnPolyobject_.first());
        pointItem->regenerate();
    }
}

void
rce::gui::RImageMarkerScene::
removePoint(const int idx)
{
    if(mode_ == DrawPolygonMode)
    {
        removeHandle(idx);

        drawnPolyobject_.remove(idx);

        QGraphicsPolygonItem *polygonItem = static_cast<QGraphicsPolygonItem *>(drawnItem_);
        QPolygonF polygon = drawnPolyobject_;
        closePolygon(polygon);

        polygonItem->setPolygon(polygon);
        emit editedPolygon(drawnItemID_);
    }
    else if(mode_ == DrawPolylineMode)
    {
        removeHandle(idx);

        drawnPolyobject_.remove(idx);

        rce::gui::RGraphicsPolylineItem *polylineItem = static_cast<rce::gui::RGraphicsPolylineItem *>(drawnItem_);

        polylineItem->setPolyline(drawnPolyobject_);
        polylineItem->regenerate();

        emit editedPolyline(drawnItemID_);
    }
    else
    {
        // nothing
    }
}

void
rce::gui::RImageMarkerScene::
removeHandle(const int idx)
{
    delete handles_[idx];
    handles_.remove(idx);
    for(int i = idx;
        i < handles_.size();
        ++i)
    {
        handles_[i]->setData(RCE_ITEM_ID_DATA_INDEX, i);
    }
}

void
rce::gui::RImageMarkerScene::
closePolygon(QPolygonF &polygon)
{
    if((polygon.size() > 1) &&
       (polygon.first() != polygon.last()))
    {
        polygon.push_back(polygon.first());
    }

}

void
rce::gui::RImageMarkerScene::
openPolygon(QPolygonF &polygon)
{
    if((polygon.size() > 1) &&
       (polygon.first() == polygon.last()))
    {
        polygon.pop_back();
    }
}

void
rce::gui::RImageMarkerScene::
getItemsAtPosition(const QRectF &sceneRect,
                   QVector<quint32> &polygons,
                   QVector<quint32> &polylines,
                   QVector<quint32> &points,
                   const QTransform &viewTransform)
{
    polygons.clear();
    polylines.clear();
    points.clear();

    QList<QGraphicsItem *> itemsUnderPos = items(sceneRect,
                                                 Qt::IntersectsItemBoundingRect,
                                                 Qt::DescendingOrder,
                                                 viewTransform);

    bool selectPolylines = (mode_ == DrawPolylineMode) || (mode_ == PickPolylineMode) || (mode_ == PickAnyMode);
    bool selectPolygons = (mode_ == DrawPolygonMode) || (mode_ == PickPolygonMode) || (mode_ == PickAnyMode);
    bool selectPoints = (mode_ == DrawPointMode) || (mode_ == PickPointMode) || (mode_ == PickAnyMode);

    for(int i = 0;
        i < itemsUnderPos.size();
        ++i)
    {
        if(!itemsUnderPos[i]->data(RCE_ITEM_TYPE_DATA_INDEX).isNull())
        {
            switch(static_cast<GraphicsItemType>(itemsUnderPos[i]->data(RCE_ITEM_TYPE_DATA_INDEX).toUInt()))
            {
                case rce::gui::RImageMarkerScene::UknownType:
                    break;
                case rce::gui::RImageMarkerScene::HandleType:
                    break;
                case rce::gui::RImageMarkerScene::PolygonType:
                    {
                        if(selectPolygons && itemsUnderPos[i]->shape().contains(sceneRect.center()))
                        {
                            polygons.push_back(itemsUnderPos[i]->data(RCE_ITEM_ID_DATA_INDEX).toUInt());
                        }
                    }
                    break;
                case rce::gui::RImageMarkerScene::PolylineType:
                    {
                        const rce::gui::RGraphicsPolylineItem* lineItem = static_cast<const rce::gui::RGraphicsPolylineItem*>(itemsUnderPos[i]);
                        if(selectPolylines && lineItem->lineShape().intersects(sceneRect))
                        {
                            polylines.push_back(itemsUnderPos[i]->data(RCE_ITEM_ID_DATA_INDEX).toUInt());
                        }
                    }
                    break;
                case rce::gui::RImageMarkerScene::PointType:
                    {
                        if(selectPoints)
                        {
                            points.push_back(itemsUnderPos[i]->data(RCE_ITEM_ID_DATA_INDEX).toUInt());
                        }
                    }
                    break;

            }
        }
    }
}

QString
rce::gui::RImageMarkerScene::
getPolygonDisplayName(quint32 id) const
{
    return tr("Polygon %1").arg(id);
}

QString
rce::gui::RImageMarkerScene::
getPolylineDisplayName(quint32 id) const
{
    return tr("Polyline %1").arg(id);
}

QString
rce::gui::RImageMarkerScene::
getPointDisplayName(quint32 id) const
{
    return tr("Point %1").arg(id);
}

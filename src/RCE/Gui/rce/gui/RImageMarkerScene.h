#ifndef RCE_GUI_RIMAGEMARKERSCENE_H
#define RCE_GUI_RIMAGEMARKERSCENE_H

#include <QMap>

#include "rce/gui/RImageWidgetScene.h"

namespace rce {
    namespace gui {

        class RGraphicsPolylineItem;
        class RGraphicsCrossItem;

        class RImageMarkerScene : public rce::gui::RImageWidgetScene
        {
            Q_OBJECT

        public:
            enum RMode
            {
                DrawPolygonMode,
                PickPolygonMode,
                DrawPolylineMode,
                PickPolylineMode,
                DrawPointMode,
                PickPointMode,
                PickAnyMode
            };


            // every added item to the scene must containt in setData/data at idx = 0 identification of type
            // every polygon and polyline must contain at idx = 1 also its id (see drawPoly***Object).
            enum GraphicsItemType : quint32
            {
                UknownType = 0,
                HandleType = 1,
                PolygonType = 2,
                PolylineType = 3,
                PointType = 4
            };

            RImageMarkerScene(QObject *parent = NULL);

            virtual ~RImageMarkerScene();

            QPolygonF getPolygonShape(quint32 id);
            QPolygonF getPolylineShape(quint32 id);
            QPointF getPoint(quint32 id);


        signals:
            void pickedPolygon(quint32 id);
            void pickedPolyline(quint32 id);
            void pickedPoint(quint32 id);
            void editedPolygon(quint32 id);
            void editedPolyline(quint32 id);
            void editedPoint(quint32 id);
            void pickedNothing();

        public slots:

            void addPolygonObject(const QPolygonF &polygon,
                                   const QBrush &brush,
                                   const QPen &pen,
                                   quint32 id);
            void addPolylineObject(const QPolygonF &polyline,
                                   const QPen &pen,
                                   quint32 id);
            void addPointObject(const QPointF &point,
                                 const QPen &pen,
                          double size,
                                 quint32 id);

            void setPolylinePen(quint32 id,
                                const QPen &pen,
                                double penWidth);

            void setPolygonBrushAndPen(quint32 id,
                                       const QBrush &brush,
                                       const QPen &pen);
            void setPointPen(quint32 id,
                             const QPen &pen);
            void setPointSize(quint32 id,
                              double size);

            void editPolygon(quint32 id);
            void editPolyline(quint32 id);
            void editPoint(quint32 id);

            void deleteObject(quint32 id);
            void deletePolygon(quint32 id);
            void deletePolyline(quint32 id);
            void deletePoint(quint32 id);

            void setMode(RMode mode);

//            Q_DECL_DEPRECATED void highlightItem(quint32 id); // DEPRECATED
//            void highlightPolygon(quint32 id);
//            void cancelPolygonHighlight(quint32 id);
//            void highlightPolyline(quint32 id);
//            void cancelPolylineHighlight(quint32 id);
//            void highlightPoint(quint32 id);
//            void cancelPointHighlight(quint32 id);
//            void cancelHighlight();

            void setAllPolylinesVisible(bool value);
            void setAllPolygonsVisible(bool value);
            void setAllPointsVisible(bool value);

            void setPolygonVisble(quint32 id, bool value);
            void setPolylineVisible(quint32 id, bool value);
            void setPointVisible(quint32 id, bool value);

            void clear();


        protected slots:

            void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
            void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
            void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
            void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

        protected: // methods
            void finishCurrentMode();
            void addManipulationHandles();
            void removeManipulationHandles();
            void addControlPoint(const QPointF &position);
            void addHandle(const QPointF &position);
            void movePoint(const QPointF &position);
            void removePoint(const int idx);
            void removeHandle(const int idx);

            void closePolygon(QPolygonF &polygon);
            void openPolygon(QPolygonF &polygon);


            void getItemsAtPosition(const QRectF &sceneRect,
                                    QVector<quint32> &polygons,
                                    QVector<quint32> &polylines,
                                    QVector<quint32> &points,
                                    const QTransform &viewTransform);

            virtual QString getPolygonDisplayName(quint32 id) const;
            virtual QString getPolylineDisplayName(quint32 id) const;
            virtual QString getPointDisplayName(quint32 id) const;
        protected:  // attributes
            RMode mode_;


            QPolygonF drawnPolyobject_;
            QGraphicsItem *drawnItem_;
            QVector<QGraphicsRectItem *> handles_;
            quint32 drawnItemID_;

            int activeHandleIdx_;
            bool itemEdited_;

            QGraphicsItem *pickedItem_;

            QMap<quint32, QGraphicsPolygonItem *> polygonItems_;
            QMap<quint32, rce::gui::RGraphicsPolylineItem *> polylineItems_;
            QMap<quint32, rce::gui::RGraphicsCrossItem *> pointItems_;


        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RIMAGEMARKERSCENE_H

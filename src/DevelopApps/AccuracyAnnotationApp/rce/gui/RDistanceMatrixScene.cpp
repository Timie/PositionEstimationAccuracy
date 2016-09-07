#include "RDistanceMatrixScene.h"

#include "rce/gui/RGraphicsFlagItem.h"

#include <QPointF>
#include <QString>

rce::gui::RDistanceMatrixScene::
RDistanceMatrixScene(QObject *parent):
    QGraphicsScene(parent)
{

}

void
rce::gui::RDistanceMatrixScene::
showPoints(const std::vector<cv::Point2d> &points,
           const rce::accuracy::RDistanceMatrix &m)
{
    clearScene();

//    addItem(new rce::gui::RGraphicsFlagItem(QPointF(0,0),
//                                            "This is Centre",
//                                            QColor(Qt::black)));


    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();


    for(int i = 0;
        i < points.size();
        ++i)
    {
        rce::gui::RGraphicsFlagItem *flagItem = new rce::gui::RGraphicsFlagItem(QPointF(-points[i].x,
                                                                                        points[i].y),
                                                                                tr("%1: %2;%3")
                                                                                .arg(i)
                                                                                .arg(points[i].x, 0, 'f', 2)
                                                                                .arg(points[i].y, 0, 'f', 2));

        flagItem->regenerate();
        flagItems_.push_back(flagItem);
        addItem(flagItem);

        if(minX > -points[i].x)
        {
            minX = -points[i].x;
        }
        if(minY > points[i].y)
        {
            minY = points[i].y;
        }
        if(maxX < -points[i].x)
        {
            maxX = -points[i].x;
        }
        if(maxY < points[i].y)
        {
            maxY = points[i].y;
        }
    }

    setSceneRect(minX, minY,
                 maxX - minX, maxY - minY);

    for(int i = 0;
        i < points.size();
        ++i)
    {
        for(int j = i+1;
            j < points.size();
            ++j)
        {
            if((m.getNumOfPlaces() == points.size()) &&
                m.isDistanceKnown(i,j))
            {
                QGraphicsLineItem *lineItem = new QGraphicsLineItem(-points[i].x,
                                                                    points[i].y,
                                                                    -points[j].x,
                                                                    points[j].y);
                QPen pen = lineItem->pen();
                pen.setCosmetic(true);
                lineItem->setPen(pen);

                rce::gui::RGraphicsFlagItem *flagItem = new rce::gui::RGraphicsFlagItem(lineItem->boundingRect().center(),
                                                                                        tr("%1;%2").arg(QString::number(m.getDistance(i,j), 'f', 2))
                                                                                                        .arg(lineItem->line().length()));
                flagItem->regenerate();

                lineItems_.push_back(lineItem);
                addItem(lineItem);
                addItem(flagItem);
            }
        }
    }
}

void
rce::gui::RDistanceMatrixScene::
clearScene()
{
    this->clear();
    flagItems_.clear();
    lineItems_.clear();
}

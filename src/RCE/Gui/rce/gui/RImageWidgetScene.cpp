#include "RImageWidgetScene.h"

#include <QPainter>


#include "rce/utility/RImageConversion.h"


rce::gui::RImageWidgetScene::
RImageWidgetScene(QObject *parent):
    QGraphicsScene(parent)
{

}

void
rce::gui::RImageWidgetScene::
setBackgroundImage(const cv::Mat &image)
{
    setBackgroundImage(rce::utility::opencvToQImage(image));

}

void
rce::gui::RImageWidgetScene::
setBackgroundImage(const QImage &image)
{
    backgroundImage_ = image;
    setSceneRect(0,0,
                 backgroundImage_.width(), backgroundImage_.height());

    invalidate(QRectF(), QGraphicsScene::BackgroundLayer);
}

void
rce::gui::RImageWidgetScene::
drawBackground(QPainter *painter,
               const QRectF &rect)
{
    if(!backgroundImage_.isNull())
    {
        painter->save();

        // draws just the updated part of the image to the scene
        painter->drawImage(rect, backgroundImage_, rect);

        painter->restore();
    }
    else
        QGraphicsScene::drawBackground(painter, rect);
}

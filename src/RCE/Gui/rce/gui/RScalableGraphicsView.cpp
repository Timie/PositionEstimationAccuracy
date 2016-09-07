#include "RScalableGraphicsView.h"

#include <QKeyEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QApplication>
#include <QDebug>

#include "rce/gui/RGuiUtils.h"

#define RCE_SCENE_ERROR_MSG_SIZE (2.0)

rce::gui::RScalableGraphicsView::
RScalableGraphicsView(QGraphicsScene *scene,
                      QWidget *parent):
    QGraphicsView(scene, parent),
    currentScale_(100),
    scaleMin_(10),
    scaleMax_(800),
    scaleStep_(10),
    updateBoundaryOnScale_(false)
{

}

rce::gui::RScalableGraphicsView::
~RScalableGraphicsView()
{

}

bool
rce::gui::RScalableGraphicsView::
canScaleIn() const
{
    return (currentScale_ < scaleMax_);
}

bool
rce::gui::RScalableGraphicsView::
canScaleOut() const
{
    return (currentScale_ > scaleMin_);
}

void
rce::gui::RScalableGraphicsView::
setErrorMessage(const QString &errorMessage)
{
    errorMessage_ = errorMessage;
    invalidateScene(QRectF(),
                    QGraphicsScene::ForegroundLayer);

}

void
rce::gui::RScalableGraphicsView::
setScale(int value/*, bool emitScaled*/)
{
    int newScale = qBound(scaleMin_,
                           value, // one "wheel turn step" is difference by 10 percent
                           scaleMax_);

    if(currentScale_ != newScale)
    {
        currentScale_ = newScale;

        double currentZoom = rce::gui::estimateViewScale(this);
        double diffZoom = (static_cast<double>(currentScale_) / 100.0) / currentZoom;
        scale(diffZoom,
              diffZoom);

        //qDebug() << "Scaled to" << currentScale_;

        //if(emitScaled)
        {
            if(updateBoundaryOnScale_)
            {
                updateBoundary();
            }
            emit scaled(currentScale_);
        }
    }
}

void
rce::gui::RScalableGraphicsView::
setScaleBounds(int min,
               int max)
{
    if(min <= max)
    {
        scaleMin_ = min;
        scaleMax_ = max;
        if((currentScale_ < min) ||
           (currentScale_ > max))
        {
            setScale(currentScale_); // this sets it into the bounds
        }
    }
}

void
rce::gui::RScalableGraphicsView::
setScaleStep(int value)
{
    scaleStep_ = value;
}

void
rce::gui::RScalableGraphicsView::
scaleIn()
{
    setScale(currentScale_ + scaleStep_);
}

void
rce::gui::RScalableGraphicsView::
scaleOut()
{
    setScale(currentScale_ - scaleStep_);
}

void
rce::gui::RScalableGraphicsView::
moveViewBy(QPointF delta)
{
    QScrollBar *hBar = horizontalScrollBar();
    QScrollBar *vBar = verticalScrollBar();

    if(hBar != NULL)
        hBar->setValue(hBar->value() + (isRightToLeft() ? qRound(delta.x()): qRound(-delta.x())));

    if(vBar != NULL)
        vBar->setValue(vBar->value() - qRound(delta.y()));
}

void
rce::gui::RScalableGraphicsView::
setUpdateBoundaryOnScale(bool value)
{
    updateBoundaryOnScale_ = value;
}

void
rce::gui::RScalableGraphicsView::
updateBoundary()
{
    if(scene() && viewport())
    {
    QRectF scaledRect = mapToScene(viewport()->geometry()).boundingRect();
    setSceneRect(scene()->sceneRect().topLeft().x() - scaledRect.width() / 2.0,
                 scene()->sceneRect().topLeft().y() - scaledRect.height() / 2.0,
                 scene()->sceneRect().width() + scaledRect.width(),
                 scene()->sceneRect().height() + scaledRect.height());
    }
}
void
rce::gui::RScalableGraphicsView::
keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->key() == Qt::Key_Plus)
    {
        setScale(currentScale_ + scaleStep_);
    }
    else if(keyEvent->key() == Qt::Key_Minus)
    {
        setScale(currentScale_ - scaleStep_);
    }
    else
        QGraphicsView::keyPressEvent(keyEvent);
}

void
rce::gui::RScalableGraphicsView::
mousePressEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::MiddleButton)
    {
        dragLastPosition_ = mouseEvent->pos();

        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }

    QGraphicsView::mousePressEvent(mouseEvent);
}

void
rce::gui::RScalableGraphicsView::
mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->buttons().testFlag(Qt::MidButton))
    {
        QScrollBar *hBar = horizontalScrollBar();
        QScrollBar *vBar = verticalScrollBar();
        QPoint delta = mouseEvent->pos() - dragLastPosition_;
        dragLastPosition_ = mouseEvent->pos();
        if(hBar != NULL)
            hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));

        if(vBar != NULL)
            vBar->setValue(vBar->value() - delta.y());
    }
    QGraphicsView::mouseMoveEvent(mouseEvent);
}

void
rce::gui::RScalableGraphicsView::
mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::MiddleButton)
    {
        dragLastPosition_ = mouseEvent->pos();

        QApplication::restoreOverrideCursor();
    }

    QGraphicsView::mouseReleaseEvent(mouseEvent);
}


void
rce::gui::RScalableGraphicsView::
wheelEvent(QWheelEvent *event)
{
    setScale(currentScale_ + qRound((event->delta() / 120.0) * scaleStep_)); // one "wheel turn step" is difference by 10 percent

    event->accept();
}

void
rce::gui::RScalableGraphicsView::
drawForeground(QPainter *painter,
               const QRectF &rect) // in scene coordinates
{
    QGraphicsView::drawForeground(painter,
                                  rect);

    if(!errorMessage_.isEmpty())
    {
        painter->save();
        QFont newFont = painter->font();
        double scale = rce::gui::estimateViewScale(this);
        newFont.setPointSizeF(newFont.pointSizeF() / scale * RCE_SCENE_ERROR_MSG_SIZE);
        painter->setFont(newFont);
        QPen newPen = painter->pen();
        newPen.setColor(Qt::red);
        painter->setPen(newPen);


        painter->drawText(rect,
                          Qt::AlignCenter,
                          errorMessage_);

        painter->restore();
    }

}

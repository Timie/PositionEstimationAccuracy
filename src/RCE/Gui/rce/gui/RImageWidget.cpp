#include "RImageWidget.h"

#include "rce/gui/RImageWidgetScene.h"


rce::gui::RImageWidget::RImageWidget(RImageWidgetScene *scene, QWidget *parent):
    rce::gui::RScalableGraphicsView(NULL, parent)
{
    if(scene != NULL)
    {
        scene_ = scene;
    }
    else
    {
        scene_ = new rce::gui::RImageWidgetScene(this);
    }


    setScene(scene_);
}

void
rce::gui::RImageWidget::
showImage(const cv::Mat &image)
{
    scene_->setBackgroundImage(image);
}

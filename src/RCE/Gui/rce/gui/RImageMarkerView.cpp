#include "RImageMarkerView.h"

#include "rce/gui/RImageMarkerScene.h"

#include <QKeyEvent>

rce::gui::RImageMarkerView::
RImageMarkerView(rce::gui::RImageMarkerScene *scene,
                 QWidget *parent):
    RScalableGraphicsView(scene, parent)
{

}

rce::gui::RImageMarkerView::
~RImageMarkerView()
{

}

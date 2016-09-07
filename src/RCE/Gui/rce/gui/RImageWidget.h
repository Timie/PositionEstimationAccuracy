#ifndef RCE_GUI_RIMAGEWIDGET_H
#define RCE_GUI_RIMAGEWIDGET_H

#include "rce/gui/RScalableGraphicsView.h"

namespace cv {class Mat;}

namespace rce {
    namespace gui {

        class RImageWidgetScene;

        class RImageWidget : public rce::gui::RScalableGraphicsView
        {
        public:
            RImageWidget(rce::gui::RImageWidgetScene *scene = NULL,
                         QWidget *parent = NULL);


            void showImage(const cv::Mat &image);


        protected:
            RImageWidgetScene *scene_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RIMAGEWIDGET_H

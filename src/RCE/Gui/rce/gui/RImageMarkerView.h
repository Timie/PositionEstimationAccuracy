#ifndef RCE_GUI_RIMAGEMARKERVIEW_H
#define RCE_GUI_RIMAGEMARKERVIEW_H

#include "rce/gui/RScalableGraphicsView.h"

namespace rce {
    namespace gui {

        class RImageMarkerScene;

        class RImageMarkerView : public  rce::gui::RScalableGraphicsView
        {
            Q_OBJECT
        public:
            RImageMarkerView(RImageMarkerScene *scene = NULL,
                             QWidget *parent = NULL);

            virtual ~RImageMarkerView();

        protected slots:
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RIMAGEMARKERVIEW_H

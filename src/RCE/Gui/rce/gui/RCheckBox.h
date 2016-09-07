#ifndef RCE_GUI_RCHECKBOX_H
#define RCE_GUI_RCHECKBOX_H

#include <QCheckBox>

class QAction;

namespace rce {
    namespace gui {

        class RCheckBox : public QCheckBox
        {
            Q_OBJECT
        public:
            explicit RCheckBox(QWidget *parent = NULL);
            explicit RCheckBox(const QString &text, QWidget *parent = NULL);
            RCheckBox(QAction *action, QWidget *parent = NULL);

            virtual ~RCheckBox();

            void setDefaultAction(QAction *action);

            void setUserTristate(bool value);

        protected slots:
            void defaultActionChanged();

        protected: // methods
           virtual void nextCheckState();

        protected: // attributes
            QAction *currentDefaultAction_;

            bool userTristate_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RCHECKBOX_H

#ifndef RCE_GUI_RPUSHBUTTON_H
#define RCE_GUI_RPUSHBUTTON_H

#include <QPushButton>

class QAction;

namespace rce
{
    namespace gui
    {
        /**
         * @brief The RPushButton class extends Qt's QPushButton with QAction support
         */
        class RPushButton : public QPushButton
        {
            Q_OBJECT
        public:
            explicit RPushButton(QWidget *parent = NULL);
            explicit RPushButton(const QString &text, QWidget *parent = NULL);
            RPushButton(const QIcon& icon, const QString &text, QWidget *parent = NULL);
            explicit RPushButton(QAction *action, QWidget *parent = NULL);
            RPushButton(QAction *action, const QString &customText, QWidget *parent = NULL);

            virtual ~RPushButton();

            void setCustomText(const QString &text);



            void setDefaultAction(QAction *action);


        protected slots:
            void defaultActionChanged();

        protected: // attributes
            QAction *currentDefaultAction_;

            QString text_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RPUSHBUTTON_H

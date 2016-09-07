#include "RPushButton.h"

#include <QAction>


#define RCE_RPUSHBUTTON_SET_GEOMETRY setGeometry(8,30,116,8);

rce::gui::RPushButton::
RPushButton(QWidget *parent):
    QPushButton(parent),
    currentDefaultAction_(NULL)
{
    RCE_RPUSHBUTTON_SET_GEOMETRY
}

rce::gui::RPushButton::
RPushButton(const QString &text,
            QWidget *parent):
    QPushButton(text, parent),
    currentDefaultAction_(NULL)
{
    RCE_RPUSHBUTTON_SET_GEOMETRY

}

rce::gui::RPushButton::
RPushButton(const QIcon &icon,
            const QString &text,
            QWidget *parent):
    QPushButton(icon, text, parent),
    currentDefaultAction_(NULL)
{
    RCE_RPUSHBUTTON_SET_GEOMETRY

}

void
rce::gui::RPushButton::
setCustomText(const QString &text)
{
    text_ = text;
    if((text_.isNull()) && (currentDefaultAction_ != NULL))
    {
        QPushButton::setText(currentDefaultAction_->text());
    }
    else
    {
        QPushButton::setText(text_);
    }
}

rce::gui::RPushButton::
RPushButton(QAction *action,
            QWidget *parent):
    QPushButton(parent),
    currentDefaultAction_(NULL)
{
    RCE_RPUSHBUTTON_SET_GEOMETRY
    setDefaultAction(action);
}

rce::gui::RPushButton::
RPushButton(QAction *action,
            const QString &customText,
            QWidget *parent):
    QPushButton(parent),
    currentDefaultAction_(NULL)
{
    RCE_RPUSHBUTTON_SET_GEOMETRY
    setCustomText(customText);
    setDefaultAction(action);
}

rce::gui::RPushButton::
~RPushButton()
{

}

void
rce::gui::RPushButton::
setDefaultAction(QAction *action)
{
    if(currentDefaultAction_ != NULL)
    {
        this->disconnect(currentDefaultAction_);
        currentDefaultAction_->disconnect(this);
        removeAction(currentDefaultAction_);
    }

    if(action != NULL)
    {
        setIcon(action->icon());
        if(text_.isNull())
        {
            QPushButton::setText(action->text());
        }
        setFont(action->font());
        setToolTip(action->toolTip());
        setEnabled(action->isEnabled());
        setVisible(action->isVisible());
        addAction(action);

        connect(action, SIGNAL(changed()),
                this, SLOT(defaultActionChanged()));

        connect(this, SIGNAL(clicked()),
                action, SLOT(trigger()));
    }

    currentDefaultAction_ = action;
}

void
rce::gui::RPushButton::
defaultActionChanged()
{
    if(currentDefaultAction_ != NULL)
    {
        setIcon(currentDefaultAction_->icon());
        if(text_.isNull())
        {
            QPushButton::setText(currentDefaultAction_->text());
        }
        setFont(currentDefaultAction_->font());
        setToolTip(currentDefaultAction_->toolTip());
        setEnabled(currentDefaultAction_->isEnabled());
        setVisible(currentDefaultAction_->isVisible());
    }
}


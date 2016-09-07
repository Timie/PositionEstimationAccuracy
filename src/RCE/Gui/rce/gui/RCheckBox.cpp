#include "RCheckBox.h"

#include <QAction>

rce::gui::RCheckBox::
RCheckBox(QWidget *parent):
    QCheckBox(parent),
  currentDefaultAction_(NULL),
    userTristate_(true)
{

}

rce::gui::RCheckBox::
RCheckBox(const QString &text,
          QWidget *parent):
    QCheckBox(text, parent),
    currentDefaultAction_(NULL),
    userTristate_(true)
{

}

rce::gui::RCheckBox::
RCheckBox(QAction *action,
          QWidget *parent):
    QCheckBox(parent),
    currentDefaultAction_(NULL),
    userTristate_(true)
{
    setDefaultAction(action);
}

rce::gui::RCheckBox::~RCheckBox()
{

}

void rce::gui::RCheckBox::setDefaultAction(QAction *action)
{
    if(currentDefaultAction_ != NULL)
    {
        this->disconnect(currentDefaultAction_);
        currentDefaultAction_->disconnect(this);
    }


    if(action != NULL)
    {
        setIcon(action->icon());
        setText(action->text());
        setFont(action->font());
        setToolTip(action->toolTip());
        setEnabled(action->isEnabled());
        setVisible(action->isVisible());
        setCheckable(action->isCheckable());
        setChecked(action->isChecked());

        connect(action, SIGNAL(changed()),
                this, SLOT(defaultActionChanged()));

        connect(this, SIGNAL(clicked(bool)),
                action, SLOT(setChecked(bool)));
    }

    currentDefaultAction_ = action;
}

void
rce::gui::RCheckBox::
setUserTristate(bool value)
{
    userTristate_ = value;
}

void
rce::gui::RCheckBox::
defaultActionChanged()
{
    if(currentDefaultAction_ != NULL)
    {
        setIcon(currentDefaultAction_->icon());
        setText(currentDefaultAction_->text());
        setFont(currentDefaultAction_->font());
        setToolTip(currentDefaultAction_->toolTip());
        setEnabled(currentDefaultAction_->isEnabled());
        setVisible(currentDefaultAction_->isVisible());
        setCheckable(currentDefaultAction_->isCheckable());
        setChecked(currentDefaultAction_->isChecked());
    }
}

void
rce::gui::RCheckBox::
nextCheckState()
{
    if(userTristate_ || (!isTristate()))
    {
        QCheckBox::nextCheckState();
    }
    else
    {
        if(isTristate())
        {
            setCheckState(checkState() == Qt::Unchecked? Qt::Checked : Qt::Unchecked);
        }
        else
        {
            QCheckBox::nextCheckState();
        }
    }
}




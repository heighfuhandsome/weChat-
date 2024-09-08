#include "switchlabel.h"
#include "global.h"
SwitchLabel::SwitchLabel(QString open, QString close, QWidget *parent):QLabel(parent),is_open_(false)
{
    state_open_ = open;
    state_close_ = close;
    setProperty("state",state_close_);
}

SwitchLabel::SwitchLabel(QWidget *parent):QLabel(parent),is_open_(false)
{

}

void SwitchLabel::setState(QString open, QString close)
{
    state_open_ = open;
    state_close_ = close;
    updateStyle();
}

void SwitchLabel::open()
{
    is_open_ = true;
    updateStyle();
}

void SwitchLabel::close()
{
    is_open_ = false;
    updateStyle();
}

void SwitchLabel::mousePressEvent(QMouseEvent *e)
{
    is_open_ = !is_open_;
    updateStyle();
    emit notifyClicked(this,is_open_,text());
}

void SwitchLabel::updateStyle()
{
    if(is_open_)
        setProperty("state",state_open_);
    else
        setProperty("state",state_close_);
    Global::repolish(this);
}

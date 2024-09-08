#include "radiobutton.h"
#include <QPainter>
#include <QMouseEvent>

RadioButton::RadioButton(QWidget *parent) : QWidget(parent),is_selected_(false)
{
}

void RadioButton::setImages(QString normal, QString selected)
{
   normal_ = normal;
   selected_ = selected;
   icon_.load(normal_);
   update();
}

void RadioButton::setSelected(bool selected)
{
    is_selected_ = selected;
    if(selected)
        emit notifySelected(this);
    if(is_selected_)
        icon_.load(selected_);
    else
        icon_.load(normal_);
    update();

}

void RadioButton::mousePressEvent(QMouseEvent *e)
{
    setSelected(true);
    QWidget::mousePressEvent(e);
}

void RadioButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.drawPixmap(rect(),icon_);
}

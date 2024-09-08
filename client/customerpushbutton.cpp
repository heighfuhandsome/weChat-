#include "customerpushbutton.h"
#include <QPainter>
void CustomerPushButton::setImages(QString normal, QString hover, QString press)
{
    normal_ = normal;
    hover_ = hover;
    press_ = press;
    pixmap_.load(normal);
    update();
}

CustomerPushButton::CustomerPushButton(QWidget *parent):QPushButton(parent),show_red_point_(false)
{
    setCursor(Qt::ClosedHandCursor);
}

void CustomerPushButton::enterEvent(QEnterEvent *e)
{
    pixmap_.load(hover_);
    update();
    QWidget::enterEvent(e);
}

void CustomerPushButton::leaveEvent(QEvent *e)
{
    pixmap_.load(normal_);
    update();
    QWidget::leaveEvent(e);
}

void CustomerPushButton::mousePressEvent(QMouseEvent *e)
{
    pixmap_.load(press_);
    update();
    QPushButton::mousePressEvent(e);
}

void CustomerPushButton::mouseReleaseEvent(QMouseEvent *e)
{
    pixmap_.load(hover_);
    update();
    QPushButton::mouseReleaseEvent(e);
}

void CustomerPushButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.drawPixmap(rect(),pixmap_);

    if(show_red_point_){
        p.setPen(Qt::NoPen);
        p.setBrush(QBrush(Qt::red));
        p.drawEllipse(rect().right()-10, rect().top(), 10, 10);
    }
}

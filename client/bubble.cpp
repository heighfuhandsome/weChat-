#include "bubble.h"
#include <QPainter>
#include <QBrush>
#include <QRect>
#define  WIDTH_SANJIAO 8
Bubble::Bubble(MSG_OWNER owner, QWidget *parent):QWidget(parent),owner_(owner)
{
    layout_ = new QHBoxLayout(this);
    if(owner == MSG_OWNER::MYSELF){
        layout_->setContentsMargins(5,3,3+WIDTH_SANJIAO,3);
    }else{
        layout_->setContentsMargins(3 + WIDTH_SANJIAO,3,5,3);
    }
}

void Bubble::setWidget(QWidget *w)
{
    if(layout_->count() >0 )
        return;
    layout_->addWidget(w);
    w->setParent(this);
}

void Bubble::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(Qt::PenStyle::NoPen);
    p.setRenderHint(QPainter::Antialiasing,true);

    if(owner_ == MSG_OWNER::MYSELF){
        p.setBrush(QBrush(Qt::green));
        QRect rectangle(rect().left(),rect().top(),rect().width() - WIDTH_SANJIAO,rect().height());
        p.drawRoundedRect(rectangle,5,5);

        QPoint points[]{
            {rect().left() + rect().width()- WIDTH_SANJIAO,rect().top()},
            {rect().left() + rect().width()- WIDTH_SANJIAO,rect().bottom()},
            {rect().left() + rect().width(),rect().top() + rect().height()/2}
        };
        p.drawPolygon(points,3);

    }else{
        p.setBrush(QBrush(Qt::white));
        QRect rectangle(rect().left() + WIDTH_SANJIAO,rect().top(),rect().width() - WIDTH_SANJIAO,rect().height());
        p.drawRoundedRect(rectangle,5,5);

        QPoint points[]{
            {rect().left()+WIDTH_SANJIAO,rect().bottom() },
            {rect().left()+WIDTH_SANJIAO,rect().top() },
            {rect().left(), rect().top()+rect().height()/2 }
        };
        p.drawPolygon(points,3);
    }

    return QWidget::paintEvent(e);
}

#include "customerscrollwidget.h"
#include <QScrollBar>
#include <QEvent>
#include <QtDebug>
#include <QStyle>
#include <QFile>
CustomerScrollWidget::CustomerScrollWidget(QWidget *parent):QWidget(parent),auto_scroll_(true)
{
    setAttribute(Qt::WA_StyledBackground,true);
    auto layout_main = new QVBoxLayout(this);
    layout_main->setContentsMargins(0,0,0,0);
    scroll_area_ = new QScrollArea(this);
    layout_main->addWidget(scroll_area_);

    widget_content_ = new QWidget(scroll_area_);
    layout_content_ = new QVBoxLayout(widget_content_);
    layout_content_->setContentsMargins(0,0,0,0);
    layout_content_->setSpacing(0);
    layout_content_->addWidget(new QWidget(),100000);

    scroll_area_->setWidget(widget_content_);
    scroll_area_->setWidgetResizable(true);

    scroll_area_->verticalScrollBar()->hide();
    scroll_area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    scroll_area_->viewport()->installEventFilter(this);
}

void CustomerScrollWidget::appendWidget(QString id, QWidget *w)
{
    if(wids_.contains(id))
        return;

   w->setParent(widget_content_);
   scroll_area_->ensureWidgetVisible(w);
   layout_content_->insertWidget(layout_content_->count()-1,w);
   wids_.insert(id,w);
}

void CustomerScrollWidget::removeWidget(QWidget *w)
{
    auto begin = wids_.begin();
    while (begin != wids_.end()) {
        if(w == begin.value()){
            layout_content_->removeWidget(w);
            delete w;
            wids_.erase(begin);
            return;
        }
        begin++;
    }
}

void CustomerScrollWidget::removeWidget(QString id)
{
    layout_content_->removeWidget(wids_[id]);
    auto wid = wids_[id];
    wids_.erase(wids_.find(id));
    delete wid;
}

void CustomerScrollWidget::setEnanleAutoScore()
{

    connect(scroll_area_->verticalScrollBar(),&QScrollBar::rangeChanged,this,[this]{
        if(auto_scroll_){
            auto vertical_bar = scroll_area_->verticalScrollBar();
            vertical_bar->setSliderPosition(vertical_bar->maximum());
        }
    });

}



bool CustomerScrollWidget::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == scroll_area_->viewport())
    {
        switch (e->type()) {
        case QEvent::Enter:if(scroll_area_->verticalScrollBar()->maximum() >0){scroll_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);scroll_area_->verticalScrollBar()->setVisible(true);break; }
        case QEvent::Leave:scroll_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff) ;break;
        default:break;
        }
    }
    return QWidget::eventFilter(obj,e);
}

#ifndef BUBBLE_H
#define BUBBLE_H

#include "global.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QPaintEvent>
class Bubble : public QWidget
{
    Q_OBJECT
public:
    explicit Bubble(MSG_OWNER owner,QWidget *parent = nullptr);
    void setWidget(QWidget *w);
protected:
    virtual void paintEvent(QPaintEvent *e) override;
signals:
private:
    MSG_OWNER owner_;
    QWidget *content_widget_;
    QHBoxLayout *layout_;
};

#endif // BUBBLE_H

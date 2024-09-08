#include "frienditembase.h"
#include "ui_frienditembase.h"
#include <QPixmap>
FriendItemBase::FriendItemBase(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendItemBase)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    layout()->setAlignment(ui->red_point,Qt::AlignTop);
    QPixmap map;
    map.load(":/images/red_point.png");
    ui->red_point->setScaledContents(true);
    ui->red_point->setPixmap(map);
    ui->red_point->hide();
    layout()->setAlignment(ui->red_point,Qt::AlignTop);
    ui->head->setScaledContents(true);
}

void FriendItemBase::setRemakr(const QString &remark)
{
    ui->remark->setText(remark);
    remark_ = remark;
}

void FriendItemBase::setHead(const QString &head)
{
    QPixmap pixmap(head);
    ui->head->setPixmap(pixmap);
}

QLabel *FriendItemBase::head()
{
    return ui->head;
}

void FriendItemBase::showReadPoint(bool visible)
{
    ui->red_point->setHidden(!visible);
}


FriendItemBase::~FriendItemBase()
{
    delete ui;
}

void FriendItemBase::mousePressEvent(QMouseEvent *e)
{
    emit clicked(this);
    QWidget::mousePressEvent(e);
}




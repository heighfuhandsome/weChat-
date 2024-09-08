#include "searchwid.h"
#include "ui_searchwid.h"
#include <QPixmap>
#include <QFile>
SearchWid::SearchWid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchWid)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);

    //qss
    QFile qss(":/style/searchwid.qss");
    qss.open(QIODevice::ReadOnly);
    setStyleSheet(qss.readAll());
    qss.close();

    initUi();
}

void SearchWid::setHintVisible(bool visible)
{
    ui->hint->setVisible(visible);
}

void SearchWid::setAddFrinedVisible(bool visible)
{
    ui->add_friend_wid->setVisible(visible);
}


SearchWid::~SearchWid()
{
    delete ui;
}

void SearchWid::mousePressEvent(QMouseEvent *e)
{
    auto g_pos = mapToGlobal(e->pos());
    if(ui->add_friend_wid->rect().contains(ui->add_friend_wid->mapFromGlobal(g_pos))){
        emit searchUserClicked();
        return;
    }
    QWidget::mousePressEvent(e);
}

void SearchWid::initUi()
{
    icon_.load(":/images/search_user.png");
    icon_ = icon_.scaled(20,20,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    // ui->icon->setScaledContents(true);
    ui->icon->setPixmap(icon_);
    ui->icon->setAlignment(Qt::AlignCenter);
}

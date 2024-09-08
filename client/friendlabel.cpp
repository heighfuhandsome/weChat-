#include "friendlabel.h"
#include "ui_friendlabel.h"
#include <QFontMetrics>
#include <QStyleOption>
FriendLabel::FriendLabel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendLabel)
{
    ui->setupUi(this);
    layout()->setContentsMargins(0,0,0,0);
    setAttribute(Qt::WA_StyledBackground,true);
    layout()->setAlignment(Qt::AlignHCenter);
    initSignal();
}

FriendLabel::~FriendLabel()
{
    delete ui;
}

void FriendLabel::setText(const QString &text)
{
    QStyleOption opt;
    opt.initFrom(ui->hint_label);
    text_ = text;
    ui->hint_label->setText(text);
    auto font_metrics = opt.fontMetrics;
    setFixedSize(font_metrics.horizontalAdvance(text) + ui->btn_close->width()+6, font_metrics.height()+6);
    width_ = size().width();
    height_ = size().height();
}

QString FriendLabel::text()
{
    return text_;
}

int FriendLabel::width()
{
    return width_;
}

int FriendLabel::heigh()
{
    return height_;
}

void FriendLabel::initSignal()
{
    connect(ui->btn_close,&QPushButton::clicked,this,[=]{
        emit notifyClose(this);
    });


}

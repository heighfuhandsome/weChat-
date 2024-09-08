#include "messageitembase.h"
#include "ui_messageitembase.h"
#include <QPushButton>
#include <QPixmap>

MessageItemBase::MessageItemBase(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MessageItemBase)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    ui->head->setScaledContents(true);
    initSignal();
}

MessageItemBase::~MessageItemBase()
{
    delete ui;
}

void MessageItemBase::setMsgType(MSG_TYPE type)
{
    msg_type_ = type;
    if(msg_type_ == MSG_TYPE::APPLY_FRIEND){
        ui->label_hint->hide();
        ui->time->hide();
    }else if(msg_type_ == MSG_TYPE::CHAT_MSG){
        ui->btn_accept->hide();
        ui->btn_rejuct->hide();
        ui->label_hint->hide();
    }
}

void MessageItemBase::setMsg(const QString &msg)
{
    ui->msg->setText(msg);
}

void MessageItemBase::setRemark(const QString &msg)
{
    ui->remark->setText(msg);
}

void MessageItemBase::setTime(const QString &msg)
{
    ui->time->setText(msg);
}

void MessageItemBase::setHint(const QString &hint)
{
    ui->label_hint->setText(hint);
}

void MessageItemBase::setHead(const QString &head)
{
    QPixmap pixmap;
    pixmap.load(head);
    ui->head->setScaledContents(true);
    ui->head->setPixmap(pixmap);
}

QLabel *MessageItemBase::getHead()
{
    return ui->head;
}

QString MessageItemBase::remark()
{
    return ui->remark->text();
}

void MessageItemBase::mousePressEvent(QMouseEvent *e)
{
    emit clicked(this);
    QWidget::mousePressEvent(e);
}

void MessageItemBase::initSignal()
{
    connect(ui->btn_accept,&QPushButton::clicked,this,[this]{
        emit clicked(this,true);
    });

    connect(ui->btn_rejuct,&QPushButton::clicked,this,[this]{
        emit clicked(this,false);
    });

}

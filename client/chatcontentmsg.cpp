#include "chatcontentmsg.h"
#include "ui_chatcontentmsg.h"
#include "global.h"
#include "frienditembase.h"
#include "chatmessageitembase.h"
#include "textbubble.h"
#include "messageitembase.h"
#include "tcpmanager.h"
#include "userinfo.h"
#include <QFile>
#include <QKeyEvent>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
ChatContentMsg::ChatContentMsg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatContentMsg),id_("-1")
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    initStyle();

    ui->btn_bq->setImages(":/images/biaoqing.png",":/images/biaoqing.png",":/images/biaoqing.png");
    ui->btn_file->setImages(":/images/file.png",":/images/file.png",":/images/file.png");
    ui->textEdit->installEventFilter(this);
    ui->scrollWid->setEnanleAutoScore();

    connect(this,&ChatContentMsg::sendMsgToNetWork,&TcpManager::GetInstance(),&TcpManager::SlotSendData);
    // TcpManager::GetInstance().registerHandler(REQ_ID::SEND_MSG,(){});
}

ChatContentMsg::~ChatContentMsg()
{
    delete ui;
}

void ChatContentMsg::switchUser(QObject *obj)
{
    FriendItemBase *fitem = nullptr;
    MessageItemBase *mitem = nullptr;
    QString old_id = id_;
    if(typeid(*obj) == typeid(FriendItemBase)){
        fitem = dynamic_cast<FriendItemBase*>(obj);
        if(fitem == nullptr)
            return;
        opposide_head_ = fitem->head()->pixmap();
        ui->title->setText(fitem->remark());
        id_ = fitem->id();
    }else if(typeid(*obj) == typeid(MessageItemBase)){
        mitem = dynamic_cast<MessageItemBase*>(obj);
        if(mitem == nullptr)
            return;
        opposide_head_ = mitem->getHead()->pixmap();
        ui->title->setText(mitem->remark());
        id_ = mitem->id();
    }

    flushMsgInterface(old_id);
}

void ChatContentMsg::onMessageCome(Global::Msg &msg)
{
    if(msg.oppoSideId_ == id_)
        flushMsgInterface(id_);
}

bool ChatContentMsg::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == ui->textEdit && e->type()==QEvent::KeyPress){
        if(reinterpret_cast<QKeyEvent*>(e)->key() == Qt::Key_Return){
            sendMsg();
            return true;
        }
    }

    return QWidget::eventFilter(obj,e);
}

void ChatContentMsg::initStyle()
{
    QFile f(":/style/chatcontentwid.qss");
    f.open(QIODevice::ReadOnly);
    setStyleSheet(f.readAll());
    Global::repolish(this);
}

void ChatContentMsg::sendMsg()
{
    auto text= ui->textEdit->toPlainText().trimmed();
    if(text== "" || id_ == "-1")
        return;


    flushMsgInterface(id_);
    ui->textEdit->clear();

    QJsonObject json;
    json["uid"] = UserInfo::GetInstance().id();
    json["token"] = UserInfo::GetInstance().token();
    json["toid"] = id_;
    json["text"] = text;
    json["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");


    Global::Msg msg;
    msg.oppoSideId_ = id_;
    msg.owner_ = MSG_OWNER::MYSELF;
    msg.text_ = text;
    msg.time_ = json["time"].toString();
    Global::getInstance().appendMsg(msg);
    emit sendMsgToNetWork(REQ_ID::SEND_MSG,QJsonDocument(json).toJson());
}

void ChatContentMsg::flushMsgInterface(QString oldId)
{
    if(oldId!= id_ &&  oldId != "-1"){
        auto &msg = Global::getInstance().getMsg(oldId);
        for(auto &e:msg){
            ui->scrollWid->removeWidget(QString::number(e.id_));
            e.wid_= nullptr;
        }
    }

    auto &msg = Global::getInstance().getMsg(id_);
    qDebug() <<   "msg" << msg.size();
    for(auto &e:msg){
        if(ui->scrollWid->containId(QString::number(e.id_)))
            continue;
        auto bubble = new TextBubble(e.owner_);
        bubble->setText(e.text_);
        ChatMessageItemBase *item;
        if(e.owner_ == MSG_OWNER::MYSELF){
            item = new ChatMessageItemBase(e.owner_,e.time_.split(" ")[1].left(5),head_,bubble,this);
        }else{
            item = new ChatMessageItemBase(e.owner_,e.time_.split(" ")[1].left(5),opposide_head_,bubble,this);
        }
        ui->scrollWid->appendWidget(QString::number(e.id_),item);
    }

}

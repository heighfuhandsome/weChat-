#include "global.h"
#include "tcpmanager.h"
#include "userinfo.h"
#include <QStyle>
#include <functional>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

unsigned long Global::msgId = 0;

Global::Global(QObject *parent) : QObject(parent)
{

}

void Global::appendMsg(Msg &msg)
{
    msg.id_ = msgId++;
    if(messages_.contains(msg.oppoSideId_)){
        messages_[msg.oppoSideId_].push_back(msg);
    }else{
        messages_.insert(msg.oppoSideId_,QVector<Msg>{msg});
    }
    emit notifyMsg(messages_[msg.oppoSideId_].back());
}

void Global::repolish(QWidget *w)
{
    w->style()->unpolish(w);
    w->style()->polish(w);
}

Global &Global::getInstance()
{
    static Global g;
    TcpManager::GetInstance().registerHandler(REQ_ID::GET_FRIEND_LIST,std::bind(&Global::onFriendListCome,&g,std::placeholders::_1));
    TcpManager::GetInstance().registerHandler(REQ_ID::GET_FRIEND_MSG,std::bind(&Global::onFriendMsgCome,&g,std::placeholders::_1));
    return g;
}

void Global::onFriendListCome(const QByteArray &data)
{
    auto json = QJsonDocument::fromJson(data).object();
    if(json["success"] == 0){
        qDebug() << "get friend list" << json["content"].toString();
        return;
    }

    auto array = json["content"].toArray();
    for(int i=0;i<array.count();i++){
        auto json = array[i].toObject();
        FriendInfo f;
        f.remark_ = json["remark"].toString();
        f.id_ = json["id"].toString();
        emit notifyFriend(f);
        if(friends_.contains(f.id_)){
            friends_[f.id_].push_back(f);
        }else{
            friends_.insert(f.id_,QVector<FriendInfo>{f});
        }
    }
}

void Global::onFriendMsgCome(const QByteArray &data)
{
    QJsonObject json = QJsonDocument::fromJson(data).object();
    if(json["success"] == 0){
        qDebug() << json["content"].toString();
        return;
    }

    QJsonArray array = json["content"].toArray();
    for(int i=0;i<array.count();i++){
        QJsonObject json = array[i].toObject();
        Msg msg;
        QString fromid = json["fromid"].toString();
        QString toid = json["toid"].toString();
        msg.text_ = json["text"].toString();
        msg.time_ = json["time"].toString();
        msg.owner_ = fromid == UserInfo::GetInstance().id() ?  MSG_OWNER::MYSELF : MSG_OWNER::OTHERS;
        msg.oppoSideId_ = fromid == UserInfo::GetInstance().id()? toid : fromid;
        appendMsg(msg);
    }
}

// void Global::getFriendMsgFromServer(QString fid)
// {
//     QJsonObject obj;
//     obj["uid"] = UserInfo::GetInstance().id();
//     obj["token"] = UserInfo::GetInstance().token();
//     obj["fid"] = fid;
//     if(skipPages_.contains(fid)){
//         obj["skipPage"] = ++skipPages_[fid];
//     }else{
//         obj["skipPage"] = 0;
//         skipPages_.insert(fid,0);
//     }

// }






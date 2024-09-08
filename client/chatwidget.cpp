#include "chatwidget.h"
#include "ui_chatwidget.h"
#include "global.h"
#include "frienditembase.h"
#include "messageitembase.h"
#include "imagecropperdialog.h"
#include "tcpmanager.h"
#include "userinfo.h"
#include <QBuffer>
#include <QFileDialog>
#include <QAction>
#include <QLineEdit>
#include <QFile>
#include <QDebug>
#include <QPalette>
#include <QStyleOption>
#include <QPainter>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <typeinfo>
#include <QJsonArray>
ChatWidget::ChatWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatWidget),isSearchMode_(false)
{

    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground,true);
    setWindowIcon(QIcon(":/images/icon.png"));


    //设置样式
    QFile f(":/style/chatwidget.qss");
    f.open(QIODevice::ReadOnly);
    QString styleSheet = f.readAll();
    setStyleSheet(styleSheet);
    Global::repolish(this);


    setWindowFlags(windowFlags() & ~ Qt::WindowContextHelpButtonHint);
    ui->side_wid->layout()->setAlignment(ui->btn_friends,Qt::AlignHCenter);
    ui->side_wid->layout()->setAlignment(ui->btn_message,Qt::AlignHCenter);
    ui->horizontalLayout_2->setAlignment(Qt::AlignVCenter);

    //初始化
    regCallBack();
    initUi();
    initSignal();
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

void ChatWidget::mousePressEvent(QMouseEvent *e)
{
    auto g_pos = mapToGlobal(e->pos());
    if(ui->head->rect().contains(ui->head->mapFromGlobal(g_pos))){
        auto img_path = QFileDialog::getOpenFileName(this,"选择头像","","Images(*.png *.jpg)");
        if(img_path.isNull())
            return;
        //图片读取成功 发送服务器
        head_ =ImageCropperDialog::getCroppedImage(img_path,600,400,CropperShape::RECT);
        QJsonObject json;
        QByteArray array;
        QBuffer buffer(&array);
        buffer.open(QIODevice::WriteOnly);
        if(img_path.endsWith("png"))
            head_.save(&buffer,"PNG");
        else
            head_.save(&buffer,"JPG");
        buffer.close();
        json["img"] = QString(array.toBase64());
        json["uid"] = UserInfo::GetInstance().id();
        json["token"] = UserInfo::GetInstance().token();
        qDebug() << "head image size " << array.toBase64().size();
        emit sendHeadImg(REQ_ID::USER_SET_HEAD,QJsonDocument(json).toJson());
    }
}

void ChatWidget::closeEvent(QCloseEvent *e)
{
    emit closed();
    QWidget::closeEvent(e);
}


void ChatWidget::OnSideBarRadionButtonSelected(QObject *o)
{
    for(int i=0;i<this->navigation_radiobutton_.size();i++){
        auto radio_button = qobject_cast<RadioButton*>(this->navigation_radiobutton_[i]);
        if(radio_button!=o)
            radio_button->setSelected(false);
    }

    switchListBar();
}

void ChatWidget::onSearchTextChange(QString text)
{
    text = text.trimmed();
    if(text == "")
        return;

    if(ui->stack_wid->currentWidget() == searchWid_)
        return;

    searchWid_->setAddFrinedVisible(false);
    searchWid_->setHintVisible(true);
    ui->stack_wid->setCurrentWidget(searchWid_);
}

void ChatWidget::onUserInfoDialogExit(bool accept)
{
    userInfoDialog_->hide();
    if(!accept)
        return;

    addFrienDiglog_ = new ApplyFriend;
    int ret = addFrienDiglog_->exec();
    while(true){
        if(ret == QDialog::Rejected)
            break;
        //发送好友申请
        auto remark = addFrienDiglog_->remark();
        auto apply = addFrienDiglog_->apply();
        QJsonObject json;
        json["uid"] = UserInfo::GetInstance().id();
        json["token"] = UserInfo::GetInstance().token();
        json["toid"] = userInfoDialog_->id();
        json["remark"] = remark;
        json["apply"] = apply;
        emit requestAddFriend(REQ_ID::REQUEST_ADD_FRIEND,QJsonDocument(json).toJson());
        break;
    }
    delete addFrienDiglog_;
}

void ChatWidget::onWhetherAcceptAddFriendBtn(QObject *obj, bool isAccept)
{
    auto item = reinterpret_cast<MessageItemBase*>(obj);
    QJsonObject json;
    json["uid"] = UserInfo::GetInstance().id();
    json["token"] = UserInfo::GetInstance().token();
    json["fromid"] = item->id();
    qDebug() << "id" << item->id();
    json["isAccept"] = isAccept;
    json["remark"] = "";

    if(!isAccept){
        callbacks_.push_back([=](const QJsonObject &json){
            friendApplyList_->removeWidget(item);
        });
        emit whetherAcceptAddFriend(REQ_ID::WHETHER_ACCEPT_ADD_FRIEND,QJsonDocument(json).toJson());
        return;
    }else{
        addFrienDiglog_ = new ApplyFriend;
        addFrienDiglog_->setApplyVisible(false);
        auto ret = addFrienDiglog_->exec();
        if(ret == QDialog::Accepted){
            json.remove("remark");
            json["remark"] = addFrienDiglog_->remark();
            callbacks_.push_back([=](const QJsonObject &json){
                if(json["success"].toInt() == 1){
                    QJsonObject obj;
                    obj["uid"] = UserInfo::GetInstance().id();
                    obj["token"] = UserInfo::GetInstance().token();
                    emit getFriendList(REQ_ID::GET_FRIEND_LIST,QJsonDocument(obj).toJson());
                    friendApplyList_->removeWidget(item);
                }
            });
            emit whetherAcceptAddFriend(REQ_ID::WHETHER_ACCEPT_ADD_FRIEND,QJsonDocument(json).toJson());
        }
        delete addFrienDiglog_;
    }
}

void ChatWidget::onNetWorkCallBack(const QByteArray &bytes)
{
    QJsonObject json = QJsonDocument::fromJson(bytes).object();
    if(json["success"] == 0)
        qDebug() << json["content"];
    if(!callbacks_.empty()){
        if(json["success"] == 1)
            callbacks_.front()(json);
        callbacks_.pop_front();
    }
}

void ChatWidget::switchChatpage(QObject *obj)
{
    ui->right_stack_wid->setCurrentWidget(chatContentWid_);
    chatContentWid_->setHead(ui->head->pixmap());

    if(typeid(*obj) == typeid(FriendItemBase)){
        auto item = dynamic_cast<FriendItemBase*>(obj);
        if(item)
            chatContentWid_->setOppoSideHead(item->head()->pixmap());
    }

    if(typeid(*obj) == typeid(MessageItemBase)){
        auto item = dynamic_cast<MessageItemBase*>(obj);
        if(item)
            chatContentWid_->setOppoSideHead(item->getHead()->pixmap());
    }
}

void ChatWidget::onFriendInfoCome(Global::FriendInfo &info)
{
    if(friendlsit_->containId(info.id_))
        return;

    auto item = new FriendItemBase;
    info.wid_ = item;
    connect(item,&FriendItemBase::clicked,this,&ChatWidget::switchChatpage);
    connect(item,&FriendItemBase::clicked,chatContentWid_,&ChatContentMsg::switchUser);
    item->setProperty("class","friendListItem");
    item->setId(info.id_);
    item->setRemakr(info.remark_);
    friendlsit_->appendWidget(item->id(),item);
    //请求头像
    QJsonObject json;
    json["uid"] = UserInfo::GetInstance().id();
    json["token"] = UserInfo::GetInstance().token();
    json["search_id"] = item->id();
    headWidgets_.append(item->head());
    emit getHeadImg(REQ_ID::GET_USER_HEAD,QJsonDocument(json).toJson());


    //请求好友
    json["fid"] = info.id_;
    json["skipPage"] = 0;
    emit getFriendMsg(REQ_ID::GET_FRIEND_MSG,QJsonDocument(json).toJson());
}

void ChatWidget::onFriendMsgCome(Global::Msg &msg)
{
    if(msgListWid_->containId(msg.oppoSideId_)){
        auto item = reinterpret_cast<MessageItemBase*>(msgListWid_->findWidget(msg.oppoSideId_));
        item->setTime(msg.time_.split(" ")[1].left(5));
        item->setMsg(msg.text_);
        return;
    }
    auto item = new MessageItemBase;
    msgListWid_->appendWidget(msg.oppoSideId_,item);
    connect(item,static_cast<void(MessageItemBase::*)(QObject*)>(&MessageItemBase::clicked),this,&ChatWidget::switchChatpage);
    connect(item,static_cast<void(MessageItemBase::*)(QObject*)>(&MessageItemBase::clicked),chatContentWid_,&ChatContentMsg::switchUser);
    item->setProperty("class","msgItem");
    item->setMsgType(MSG_TYPE::CHAT_MSG);
    item->setTime(msg.time_.split(" ")[1].left(5));
    item->setMsg(msg.text_);
    item->setId(msg.oppoSideId_);

    //请求头像
    QJsonObject json;
    json["uid"] = UserInfo::GetInstance().id();
    json["token"] = UserInfo::GetInstance().token();
    json["search_id"] = msg.oppoSideId_;
    headWidgets_.push_back(item->getHead());
    emit getHeadImg(REQ_ID::GET_USER_HEAD,QJsonDocument(json).toJson());


    //请求昵称
    callbacks_.push_back([=](const QJsonObject &json){
        if(json["success"] ==0){
            qDebug() << json["content"].toString();
            return;
        }
        item->setAccount(json["account"].toString());
        item->setRemark(json["nick"].toString());
    });
    emit searchUserById(REQ_ID::GET_USERINFO_BY_ID,QJsonDocument(json).toJson());
}


void ChatWidget::regCallBack()
{
    //初始化网络信号
    connect(this,&ChatWidget::sendHeadImg,&TcpManager::GetInstance(),&TcpManager::SlotSendData);
    connect(this,&ChatWidget::getHeadImg,&TcpManager::GetInstance(),&TcpManager::SlotSendData);
    connect(this,&ChatWidget::searchUserByAccount,&TcpManager::GetInstance(),&TcpManager::SlotSendData);
    connect(this,&ChatWidget::requestAddFriend,&TcpManager::GetInstance(),&TcpManager::SlotSendData);
    connect(this,&ChatWidget::getApplyFriendList,&TcpManager::GetInstance(),&TcpManager::SlotSendData);
    connect(this,&ChatWidget::searchUserById,&TcpManager::GetInstance(),&TcpManager::SlotSendData);
    connect(this,&ChatWidget::whetherAcceptAddFriend,&TcpManager::GetInstance(),&TcpManager::SlotSendData);
    connect(this,&ChatWidget::getFriendList,&TcpManager::GetInstance(),&TcpManager::SlotSendData);
    connect(this,&ChatWidget::getFriendMsg,&TcpManager::GetInstance(),&TcpManager::SlotSendData);



    //注册网络回调
    TcpManager::GetInstance().registerHandler(REQ_ID::WHETHER_ACCEPT_ADD_FRIEND,std::bind(&ChatWidget::onNetWorkCallBack,this,std::placeholders::_1));
    TcpManager::GetInstance().registerHandler(REQ_ID::GET_USERINFO_BY_ID,[this](const QByteArray &bytes){
        auto json = QJsonDocument::fromJson(bytes).object();
        if(!callbacks_.empty()){
            callbacks_.front()(json);
            callbacks_.pop_front();
        }
    });


    TcpManager::GetInstance().registerHandler(REQ_ID::USER_SET_HEAD,[this](const QByteArray &bytes){
        auto json = QJsonDocument::fromJson(bytes).object();
        if(json["success"] == 1){
            ui->head->setPixmap(head_);
        }else{
            qDebug() << json["content"];
        }
    });

    TcpManager::GetInstance().registerHandler(REQ_ID::GET_USER_HEAD,[this](const QByteArray &bytes){
        auto json = QJsonDocument::fromJson(bytes).object();
        if(json["success"] == 1){
            QByteArray data = QByteArray::fromBase64(json["content"].toString().toUtf8());
            if(headWidgets_.empty())
                return;
            QPixmap map;
            map.loadFromData(data);
            auto front = headWidgets_.front();
            if(typeid(*front) == typeid(QLabel)){
                dynamic_cast<QLabel*>(front)->setPixmap(map);
            }
            headWidgets_.pop_front();
        }else{
            qDebug() << json["content"];
        }
    });



    TcpManager::GetInstance().registerHandler(REQ_ID::GET_USERINFO_BY_ACCOUNT,[this](const QByteArray &bytes){
        auto json = QJsonDocument::fromJson(bytes).object();
        if(!callbacks_.empty())
        {
            callbacks_.front()(json);
            callbacks_.pop_front();
            qDebug() << "call back" << static_cast<int>(REQ_ID::GET_USERINFO_BY_ACCOUNT);
            return ;
        }

        if(json["success"] == 0 && ui->stack_wid->currentWidget() == searchWid_){
            searchWid_->setHintVisible(true);
            return;
        }else{
            searchWid_->setHintVisible(false);
            auto search_id = json["id"].toString();
            auto nick = json["nick"].toString();
            auto id = json["id"].toString();
            userInfoDialog_->setNick(nick);
            userInfoDialog_->setHeadImg(head_);
            userInfoDialog_->setId(id);
            //查找该用户的头像
            headWidgets_.push_back(userInfoDialog_->Head());
            QJsonObject json;
            json["uid"] = UserInfo::GetInstance().id();
            json["token"] = UserInfo::GetInstance().token();
            json["search_id"] = search_id;
            emit getHeadImg(REQ_ID::GET_USER_HEAD,QJsonDocument(json).toJson());
            userInfoDialog_->show();
            return;
        }
    });

    TcpManager::GetInstance().registerHandler(REQ_ID::GET_APPLY_FRIEND_LIST,[this](const QByteArray &bytes){
        QJsonObject json = QJsonDocument::fromJson(bytes).object();
        if(json["success"] == 0){
            qDebug() << json["content"].toString();
            return;
        }
        const auto &array = json["content"].toArray();
        for(int i=0;i<array.size();i++){
            auto applyFriend = new MessageItemBase;
            applyFriend->setMsgType(MSG_TYPE::APPLY_FRIEND);
            applyFriend->setMsg(array[i]["applyInfo"].toString());
            applyFriend->setId(array[i]["fromid"].toString());
            applyFriend->setProperty("class","friendApplyItem");
            connect(applyFriend,static_cast<void(MessageItemBase::*)(QObject*,bool)>(&MessageItemBase::clicked),this,&ChatWidget::onWhetherAcceptAddFriendBtn);
            friendApplyList_->appendWidget(applyFriend->id(),applyFriend);

            //请求昵称
            callbacks_.push_back([=](const QJsonObject &json){
                if(json["success"] ==0){
                    qDebug() << json["content"].toString();
                    return;
                }
                applyFriend->setAccount(json["account"].toString());
                applyFriend->setRemark(json["nick"].toString());
                qDebug() << "account" << applyFriend->account();
            });
            QJsonObject json1;
            json1["uid"] = UserInfo::GetInstance().id();
            json1["token"] = UserInfo::GetInstance().token();
            json1["search_id"] = applyFriend->id();
            emit searchUserById(REQ_ID::GET_USERINFO_BY_ID,QJsonDocument(json1).toJson());


            //请求头像
            headWidgets_.push_back(applyFriend->getHead());
            emit getHeadImg(REQ_ID::GET_USER_HEAD,QJsonDocument(json1).toJson());
        }
    });



}

void ChatWidget::initUi()
{

    //初始化头像
    ui->side_wid->layout()->setAlignment(ui->head,Qt::AlignHCenter);
    ui->head->setScaledContents(true);
    head_.load(":/images/head.png");
    head_.scaled(ui->head->width(),ui->head->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->head->setPixmap(head_);

    //在页面初始化完成前 先禁用按钮
    ui->btn_friends->setEnabled(false);
    ui->btn_message->setEnabled(false);

    //初始化所有stack_wid的边距
    ui->stack_wid->setContentsMargins(0,0,0,0);
    ui->right_stack_wid->setContentsMargins(0,0,0,0);


    //初始化侧边栏按钮和搜索框旁边的添加按钮
    this->navigation_radiobutton_<< ui->btn_message << ui->btn_friends; //添加单选按钮到按钮组
    ui->btn_add->setImages(":/images/friend_add.png",":/images/friend_add_hover.png",":/images/friend_add_hover.png");
    ui->btn_message->setImages(":/images/msg_normal.png",":/images/msg_hover.png");
    ui->btn_friends->setImages(":/images/friends_normal.png",":/images/friends_hover.png");
    ui->btn_message->setSelected(true);
    ui->btn_friends->setSelected(false);


    //初始化搜索列表
    searchWid_ = new SearchWid(ui->stack_wid);
    ui->stack_wid->addWidget(searchWid_);



    //初始化消息列表
    msgListWid_ = new CustomerScrollWidget(ui->stack_wid);
    ui->stack_wid->addWidget(msgListWid_);
    ui->stack_wid->setCurrentWidget(msgListWid_);
    qDebug() << msgListWid_->width();


    //初始化好友列表
    friendlsit_ = new CustomerScrollWidget(ui->stack_wid);
    ui->stack_wid->addWidget(friendlsit_);

    //初始化聊天界面右侧聊天内容界面
    chatContentWid_ = new ChatContentMsg(ui->right_stack_wid);
    ui->right_stack_wid->addWidget(chatContentWid_);
    ui->right_stack_wid->setCurrentWidget(chatContentWid_);

    //初始化好友申请列表
    friendApplyList_ = new CustomerScrollWidget(ui->right_stack_wid);
    ui->right_stack_wid->addWidget(friendApplyList_);


    //初始化搜索框
    ui->search_lineEdit->setPlaceholderText("搜索");
    auto searchAction = new QAction(this);
    searchAction->setIcon(QIcon(":/images/search.png"));
    ui->search_lineEdit->addAction(searchAction,QLineEdit::LeadingPosition);

    auto cleanAction = new QAction(this);
    cleanAction->setIcon(QIcon(":/images/white.png"));
    ui->search_lineEdit->addAction(cleanAction,QLineEdit::TrailingPosition);
    connect(ui->search_lineEdit,&QLineEdit::textChanged,this,[=]{
        if(ui->search_lineEdit->text() !="")
            cleanAction->setIcon(QIcon(":/images/clearn.png"));
        else
            cleanAction->setIcon(QIcon(":/images/white.png"));
    });
    connect(cleanAction,&QAction::triggered,this,[=]{
        ui->search_lineEdit->clear();
        ui->search_lineEdit->clearFocus();
        cleanAction->setIcon(QIcon(":/images/white.png"));
    });


    //初始化搜寻联系人dialog
    userInfoDialog_ = new UserInfoDialog(this);
    userInfoDialog_->Head();
}



void ChatWidget::initSignal()
{
    connect(ui->btn_friends,&RadioButton::notifySelected,this,&ChatWidget::OnSideBarRadionButtonSelected);
    connect(ui->btn_message,&RadioButton::notifySelected,this,&ChatWidget::OnSideBarRadionButtonSelected);
    connect(ui->search_lineEdit,&QLineEdit::textEdited,this,&ChatWidget::onSearchTextChange);
    connect(ui->btn_add,&QPushButton::clicked,this,[this]{
        searchWid_->setHintVisible(false);
        searchWid_->setAddFrinedVisible(true);
        if(ui->stack_wid->currentWidget() != searchWid_){
            ui->stack_wid->setCurrentWidget(searchWid_);
        }
    });
    connect(searchWid_,&SearchWid::searchUserClicked,this,[this]{
        auto text = ui->search_lineEdit->text().trimmed();
        if(text == "")
            return;
        QJsonObject json;
        json["uid"] = UserInfo::GetInstance().id();
        json["token"] = UserInfo::GetInstance().token();
        json["account"] = text;
        emit searchUserByAccount(REQ_ID::GET_USERINFO_BY_ACCOUNT,QJsonDocument(json).toJson());
    });


    connect(userInfoDialog_,&UserInfoDialog::exit,this,&ChatWidget::onUserInfoDialogExit);
    connect(&Global::getInstance(),&Global::notifyFriend,this,&ChatWidget::onFriendInfoCome);
    connect(&Global::getInstance(),&Global::notifyMsg,this,&ChatWidget::onFriendMsgCome);
    connect(&Global::getInstance(),&Global::notifyMsg,chatContentWid_,&ChatContentMsg::onMessageCome);

    ui->btn_friends->setEnabled(true);
    ui->btn_message->setEnabled(true);
}

void ChatWidget::initData()
{

    // 初始化好友列标签
    auto label1 = new QLabel("新的朋友");
    label1->setContentsMargins(5,5,5,5);
    label1->setAlignment(Qt::AlignVCenter);
    label1->setProperty("class","tag");
    friendlsit_->appendWidget("-1",label1);
    auto item = new FriendItemBase;
    item->setHead(":/images/new_friend.png");
    item->setRemakr("新的朋友");
    item->setProperty("class","friendListItem_newFriend");
    friendlsit_->appendWidget("-2",item);
    connect(item,&FriendItemBase::clicked,[this](QObject *o){
        Q_UNUSED(o);
        ui->right_stack_wid->setCurrentWidget(friendApplyList_);
    });
    auto Label2 = new QLabel("联系人");
    Label2->setContentsMargins(5,5,5,5);
    Label2->setProperty("class","tag");
    friendlsit_->appendWidget("-3",Label2);


    //从网络端请求头像
    QJsonObject json;
    json["uid"] = UserInfo::GetInstance().id();
    json["token"] = UserInfo::GetInstance().token();
    json["search_id"] = UserInfo::GetInstance().id();
    headWidgets_.push_back(ui->head);
    emit getHeadImg(REQ_ID::GET_USER_HEAD,QJsonDocument(json).toJson());

    //从服务器获取好友列表
    emit getFriendList(REQ_ID::GET_FRIEND_LIST,QJsonDocument(json).toJson());

    //从服务器获取好友申请信息
    json.remove("search_id");
    emit getApplyFriendList(REQ_ID::GET_APPLY_FRIEND_LIST,QJsonDocument(json).toJson());


    //获取好友信息



}

void ChatWidget::switchListBar()
{
    ui->search_lineEdit->clear();
    for(auto sideBar : navigation_radiobutton_){
        auto btn = reinterpret_cast<RadioButton*>(sideBar);
        if(!btn->isSelect())
            continue;

        if(btn == ui->btn_message){
            ui->stack_wid->setCurrentWidget(msgListWid_);
        }
        else{
            ui->stack_wid->setCurrentWidget(friendlsit_);
        }
        break;
    }

}

void ChatWidget::cleanJson(QJsonObject &json, QStringList keys) {
    for (auto key : keys) {
        json.remove(key);
    }
}

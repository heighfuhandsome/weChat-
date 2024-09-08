#include "widget.h"
#include "./ui_widget.h"
#include "userinfo.h"
#include "httpaccessmanager.h"
#include "tcpmanager.h"
#include "register.h"
#include <QFile>
#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget),accountIsNull_(true),pwdIsNull_(true)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/icon.png"));
    setWindowTitle("XChat");
    setAttribute(Qt::WA_StyledBackground);

    //加载qss
    QFile f(":/style/loginwid.qss");
    f.open(QIODevice::ReadOnly);
    setStyleSheet(f.readAll());



    //初始化信号
    initUi();
    initSignal();

}


void Widget::initUi()
{

    //加载头像
    layout()->setAlignment(ui->head,Qt::AlignHCenter);
    ui->head->setScaledContents(true);
    ui->head->setContentsMargins(0,0,0,0);
    QPixmap map;
    map.load(":/images/icon.png");
    ui->head->setPixmap(map);

    //登录按钮初始不可用
    ui->btn_login->setProperty("state","disable");
    ui->btn_login->setEnabled(false);

    //输入限制
    ui->account->setValidator(new QRegularExpressionValidator(QRegularExpression(R"([0-9]*)"),this));

    //提示消息居中
    ui->hint->setAlignment(Qt::AlignCenter);
}

Widget::~Widget()
{
    delete chat_widget_;
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    auto pos = mapToGlobal(e->pos());
    if(ui->label_reg->rect().contains(ui->label_reg->mapFromGlobal(pos))){
        auto reg = new Register;
        connect(reg,&Register::closed,this,&Widget::handlerRegiserClose);
        reg->move(this->pos());
        setVisible(false);
        reg->show();
    }
    QWidget::mousePressEvent(e);
}


void Widget::showTip(QString info, bool error)
{
    if(error)
        ui->hint->setProperty("state","error");
    else
        ui->hint->setProperty("state","normal");

    Global::repolish(ui->hint);
    ui->hint->setText(info);
}

void Widget::switchChatWid()
{
    chat_widget_ = new ChatWidget;
    setVisible(false);
    chat_widget_->show();
    chat_widget_->initData();
}


void Widget::on_btn_login_clicked()
{
    QJsonObject json;
    json["account"] = ui->account->text().trimmed();
    json["password"] = ui->pwd->text().trimmed();
    HttpAccessManager::GetInstance().post(REQ_ID::USER_LOGIN,MODULE_ID::LOGIN_MODULE,QJsonDocument(json));
}

void Widget::handlerResponse(REQ_ID rid, ERR_CODE error, QByteArray data)
{
    Q_UNUSED(rid);
    auto json = QJsonDocument::fromJson(data).object();
    if(error !=ERR_CODE::SUCCESS){
        showTip("网络错误",true);
        return;
    }

    if(json["success"].toInt() != 1){
        showTip(json["content"].toString(),true);
        return;
    }

    showTip("密码正确，连接服务器...",false);

    QString chatHost = json["host"].toString();
    QString chatPort = json["port"].toString();
    QString uid = json["uid"].toString();
    QString token = json["token"].toString();

    UserInfo &user_info = UserInfo::GetInstance();
    user_info.setId(uid);
    user_info.setToken(token);
    qDebug() << uid << token;
    qDebug() << chatHost;
    qDebug() << chatPort;
    TcpManager::GetInstance().connect(chatHost,chatPort.toShort());
}

void Widget::handlerRegiserClose(QObject *regiserWindow)
{
    auto reg_wid = reinterpret_cast<Register*>(regiserWindow);
    reg_wid->setVisible(false);
    move(reg_wid->pos());
    delete regiserWindow;
    setVisible(true);
}


void Widget::updateLoginBtnState()
{

    if(!accountIsNull_ && !pwdIsNull_){
        ui->btn_login->setProperty("state","enable");
        ui->btn_login->setEnabled(true);
    }else{
        ui->btn_login->setProperty("state","disable");
        ui->btn_login->setEnabled(false);
    }
    Global::repolish(ui->btn_login);
}

void Widget::switchChatWidget()
{
    chat_widget_ = new ChatWidget;
    connect(chat_widget_,&ChatWidget::closed,this,[this]{
        close();
    });
    chat_widget_->show();
    setVisible(false);
}

void Widget::initSignal()
{

    //初始化信号连接

    connect(&HttpAccessManager::GetInstance(),&HttpAccessManager::notifyLoginModule,this,&Widget::handlerResponse);
    connect(&TcpManager::GetInstance(),&TcpManager::notifyConnected,this,[this](){
        showTip("服务器连接成功,登录中...",false);
        UserInfo &user = UserInfo::GetInstance();
        QJsonObject json;
        json["uid"] = user.id();
        json["token"] = user.token();
        emit loginChatServer(REQ_ID::USER_LOGIN_CHAT_SERVER,QJsonDocument(json).toJson());
    });
    connect(&TcpManager::GetInstance(),&TcpManager::notifyDisConnected,this,[this]{
        showTip("连接已断开...",true);
    });
    connect(this,&Widget::loginChatServer,&TcpManager::GetInstance(),&TcpManager::SlotSendData);

    //注册tcpManager回掉
    TcpManager::GetInstance().registerHandler(REQ_ID::USER_LOGIN_CHAT_SERVER,[this](const QByteArray &data){
        auto json = QJsonDocument::fromJson(data).object();
        if(json["uid"].isNull() || json["token"].isNull()){
            showTip("登录失败...",true);
        }else{
            showTip("登录成功",false);
            switchChatWid();
        }
    });


    connect(ui->account,&QLineEdit::textChanged,[this](const QString &text){
        accountIsNull_ = text=="";
        updateLoginBtnState();
    });

    connect(ui->pwd,&QLineEdit::textChanged,[this](const QString &text){
        pwdIsNull_ = text=="";
        updateLoginBtnState();
    });
}



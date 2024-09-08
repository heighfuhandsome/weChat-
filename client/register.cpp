#include "register.h"
#include "ui_register.h"
#include <QToolButton>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpressionValidator>
#include <QFile>
#include <QCloseEvent>
Register::Register(QWidget *parent) :
    QWidget(parent),accountIsValid_(false),pwdIsValid_(false),nickNameIsValid_(false),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    QFile f(":/style/registerwid.qss");
    f.open(QIODevice::ReadOnly);
    setStyleSheet(f.readAll());

    initUi();
    initSignal();
    timer_ = new QTimer(this);
    connect(timer_,&QTimer::timeout,this,&Register::onTimerout);
    timer_->setSingleShot(true);

}

void Register::initUi()
{
    setWindowTitle("XChat");
    setWindowIcon(QIcon(":/images/icon.png"));

    //设置icon
    layout()->setAlignment(ui->icon,Qt::AlignHCenter);
    ui->icon->setScaledContents(true);
    QPixmap pixmap;
    pixmap.load(":/images/icon.png");
    ui->icon->setPixmap(pixmap);

    //设置按钮状态 不可用
    ui->btn_reg->setProperty("state","disable");
    ui->btn_reg->setEnabled(false);

    //设置提示位置
    ui->hint->setAlignment(Qt::AlignHCenter);


}

void Register::initSignal()
{
    connect(ui->nick,&QLineEdit::textChanged,this,[this](QString text){
        text = text.trimmed();
        bool tag = false;
        if(text.length() == 0){
            updateHint("昵称不能为空",true);
        }else if(text.length() > 10){
            updateHint("昵称不能大于10位",true);
        }else{
            updateHint("",false);
            tag = true;
        }
        nickNameIsValid_ = tag;
        updateBtnRegState();
        startTimer();
    });



    connect(ui->account,&QLineEdit::textChanged,this,[this](QString text){
        text = text.trimmed();
        bool tag = false;
        if(text.length() == 0){
            updateHint("账号不能位空",true);
        }else if(text.length() < 6 || text.length() > 11){
            updateHint("账号为6至11位数",true);
        }else{
            updateHint("",false);
            tag = true;
        }
        accountIsValid_ = tag;
        updateBtnRegState();
        startTimer();
    });

    connect(ui->password,&QLineEdit::textChanged,this,[this](QString text){
        text = text.trimmed();
        bool tag = false;
        if(text.length() == 0){
            updateHint("密码不能为空",true);
        }else if(text.length() < 6 || text.length() > 16){
            updateHint("密码个数应在6到16位之间",true);
        }else{
            updateHint("",false);
            tag = true;
        }
        pwdIsValid_ = tag;
        updateBtnRegState();
        startTimer();
    });

    connect(&HttpAccessManager::GetInstance(),&HttpAccessManager::notifyRegModule,this,&Register::HandleHttpRes) ;
    connect(ui->btn_reg,&QPushButton::clicked,this,&Register::onRegister);

}

void Register::updateHint(QString hint, bool isError)
{
    if(isError)
        ui->hint->setProperty("state","error");
    else
        ui->hint->setProperty("state","normal");
    ui->hint->setText(hint);
    Global::repolish(ui->hint);
}

void Register::onRegister()
{
    if(timer_->isActive())
        timer_->stop();
    updateHint("请求中...",false);
    QJsonObject json;
    json["account"] = ui->account->text().trimmed();
    json["password"] = ui->password->text().trimmed();
    json["nick"] = ui->nick->text().trimmed();
    HttpAccessManager::GetInstance().post(REQ_ID::USER_REGISTER,MODULE_ID::REG_MODULE,QJsonDocument(json));
}

void Register::HandleHttpRes(REQ_ID rid, ERR_CODE ec, QByteArray data)
{
    if(ec == ERR_CODE::NETWORK_ERR){
        updateHint("网络错误",true);
        return;
    }

    if(ec != ERR_CODE::SUCCESS)
    {
        qDebug() << "network error";
        return;
    }


    QJsonObject json = QJsonDocument::fromJson(data).object();
    if(json["success"] == 1){
        updateHint("注册成功",false);
        return;
    }

    updateHint(json["content"].toString(),true);
}

void Register::onTimerout()
{
    ui->hint->setText("");
}

void Register::startTimer()
{
    if(timer_->isActive())
        timer_->stop();
    timer_->start(1500);
}

Register::~Register()
{
    delete ui;
}

void Register::closeEvent(QCloseEvent *e)
{
    emit closed(this);
}

void Register::updateBtnRegState()
{
    if(nickNameIsValid_ && accountIsValid_ && pwdIsValid_)
        ui->btn_reg->setProperty("state","enable");
    else
        ui->btn_reg->setProperty("state","disable");

    ui->btn_reg->setEnabled(ui->btn_reg->property("state") == "enable");
    Global::repolish(ui->btn_reg);
}


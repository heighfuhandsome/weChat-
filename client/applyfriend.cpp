#include "applyfriend.h"
#include "ui_applyfriend.h"
#include "global.h"
#include "friendlabel.h"
#include <QFile>
#include <QKeyEvent>
#include <QScrollBar>
#include <QLineEdit>
#include <QDebug>
#include <QMouseEvent>
ApplyFriend::ApplyFriend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyFriend)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground);

    //初始化
    initData();
    initStyle();
    initUi();
    initSignal();


    ui->edit_lable->installEventFilter(this);
    ui->label_list_wid->installEventFilter(this);
    ui->scrollArea->installEventFilter(this);
}

void ApplyFriend::clean()
{
    ui->edit_remarks->clear();
    ui->edit_apply_info->clear();
}


void ApplyFriend::setApplyVisible(bool visible)
{
    ui->label_2->setVisible(visible);
    ui->edit_apply_info->setVisible(visible);
    if(!visible)
        ui->edit_apply_info->setText("aaa");
}

void ApplyFriend::showHintLabel()
{
    auto hint_label_pos_ = QPoint{5,5};
    int line =1;
    for(auto &sw_lb:hint_labels_){
        sw_lb->hide();
    }

    QFontMetrics metrics{ hint_labels_.begin().value()->font() };
    for(auto &sw_lb:hint_labels_)
    {
        int text_width = metrics.horizontalAdvance(sw_lb->text());
        int text_high = metrics.height();
        //判断是否要换行显示
        if(hint_label_pos_.x() + text_width > ui->hint_lable_wid->width() + 15){
            line++;
            //默认只显示两行
            if(line>2 && !hint_label_show_all_){
                return;
            }
            //换行
            hint_label_pos_.setX(5);
            hint_label_pos_.setY(hint_label_pos_.y() + text_high + 10);
        }

        //显示
        sw_lb->move(hint_label_pos_);
        sw_lb->show();
        //更新x坐标
        hint_label_pos_.setX(hint_label_pos_.x() + text_width + 15);
    }

}

ApplyFriend::~ApplyFriend()
{
    delete ui;
}

void ApplyFriend::onHintLabelClick(QObject *o, bool isOpen,QString text)
{
    if(isOpen){
        addFriendLabel(text.trimmed());
    }else{
        delFriendLabel(text.trimmed());
    }
}

void ApplyFriend::onFriendLabelClicked(QObject *obj)
{
    auto f_lb = reinterpret_cast<FriendLabel*>(obj);
    auto ht_lb = hint_labels_.find(f_lb->text());
    if(ht_lb != hint_labels_.end())
        ht_lb.value()->close();
    delFriendLabel(f_lb->text());
}

bool ApplyFriend::eventFilter(QObject *o, QEvent *e)
{
    if(o == ui->edit_lable && e->type() == QEvent::KeyPress)
    {//捕获label输入回车
        auto key_event = static_cast<QKeyEvent*>(e);
        if(key_event->key() == Qt::Key_Return){
            editLabelReturn();
            return true;
        }
    }else if(o == ui->label_list_wid && e->type() == QEvent::MouseButtonRelease){
        ui->edit_lable->setFocus();
    }else if(o==ui->scrollArea && e->type() == QEvent::Enter){
        auto vertical_bar = ui->scrollArea->verticalScrollBar();
        vertical_bar->setHidden(vertical_bar->maximum() == 0);
    }else if(o == ui->scrollArea && e->type() == QEvent::Leave){
        ui->scrollArea->verticalScrollBar()->hide();
    }

    return QDialog::eventFilter(o,e);
}

void ApplyFriend::mousePressEvent(QMouseEvent *e)
{
    auto gpos = mapToGlobal(e->pos());
    if(ui->add_label->rect().contains(ui->add_label->mapFromGlobal(gpos)) && ui->add_label->isVisible())
    {
        editLabelReturn();
        return;
    }
    QDialog::mousePressEvent(e);
}

void ApplyFriend::showFriendLabel()
{
    QPoint curr_pos = QPoint{2,2};
    for(auto &f_lb:friend_labels_)
        f_lb->hide();


    //记录好友标签容器初始高度
    static int lb_lt_high = ui->label_list_wid->height();
    //记录好友标签容器父容器高度
    static int container_high = ui->widget_4->height();
    // 记录滚动视图出事高度
    static int scroll_wid_high = ui->scrollAreaWidgetContents->height();

    for(auto &f_lb:friend_labels_){
        int width = f_lb->width();
        int high = f_lb->heigh();
        //判断是否需要换行
        if(width + curr_pos.x() + 5 > ui->label_list_wid->width()){
            curr_pos.setX(2);
            curr_pos.setY(high + curr_pos.y() + 5);
        }
        f_lb->move(curr_pos);
        f_lb->show();
        //更新x坐标
        curr_pos.setX(curr_pos.x() + width +5);

        //跟新标签输入框位置 先判断是否需要换行
        if(curr_pos.x() + ui->edit_lable->width() > ui->label_list_wid->width()){
            curr_pos.setX(2);
            curr_pos.setY(ui->edit_lable->height() + curr_pos.y() + 5);
        }
        ui->edit_lable->move(curr_pos);

        // 跟新标签容器 防止标签被覆盖
        if(curr_pos.y() + high +5 > lb_lt_high)
        {
            //跟新标签容器高度
            ui->label_list_wid->setFixedHeight(curr_pos.y() + high +5);
            //计算便宜了
            int offset = ui->label_list_wid->height() - lb_lt_high;
            //跟新标签容器父容器
            ui->widget_4->setFixedHeight(container_high + offset);
            //跟新滚动视图高度
            ui->scrollAreaWidgetContents->setFixedHeight(scroll_wid_high + offset);
        }
    }

    //如果没有添加标签，或者全部删除，输入框放到起始位置
    if(friend_labels_.size() == 0)
        ui->edit_lable->move(curr_pos);

}

void ApplyFriend::addHintLabel(const QString &key,bool isOpen)
{
    if(hint_labels_.contains(key))
        return;
    auto sw_lb = new SwitchLabel("open","close",ui->hint_lable_wid);
    sw_lb->setText(key);
    if(isOpen)
        sw_lb->open();
    connect(sw_lb,&SwitchLabel::notifyClicked,this,&ApplyFriend::onHintLabelClick);
    sw_lb->hide();
    hint_label_key_.push_back(key);
    hint_labels_.insert(key,sw_lb);
    showHintLabel();
}



void ApplyFriend::initData()
{
    //初始化标签
    hint_label_key_ << "同学" << "家人" << "菜鸟教程" << "C++ Primer" << "Rust 程序设计" <<
        "父与子学Python" << "nodejs开发指南" << "go 语言开发指南" <<
                                "游戏伙伴" << "金融投资" << "微信读书" << "拼多多拼友";
    for(auto &key:hint_label_key_)
        addHintLabel(key,false);
}

void ApplyFriend::initStyle()
{
    QFile f(":/style/applyfriendwid.qss");
    f.open(QIODevice::ReadOnly);
    setStyleSheet(f.readAll());
    Global::repolish(this);
}

void ApplyFriend::initUi()
{
    ui->add_label->hide();
    ui->edit_lable->setMaxLength(20);
    ui->edit_remarks->setMaxLength(15);
    ui->edit_apply_info->setMaxLength(30);

    ui->scrollArea->verticalScrollBar()->hide();
    ui->scrollArea->horizontalScrollBar()->hide();
    ui->btn_show_more->setState("open","close");
    ui->hint_label_container_wid->layout()->setAlignment(ui->btn_show_more,Qt::AlignTop);
    ui->scrollArea->setContentsMargins(0,0,0,0);
    ui->scrollArea->setWidgetResizable(true);
    showHintLabel();
}

void ApplyFriend::initSignal()
{
    //对话框拒绝按钮信号
    connect(ui->btn_cacel,&QPushButton::clicked,this,[this]{
        reject();
    });
    //对话框确定按钮信号
    connect(ui->btn_sure,&QPushButton::clicked,this,[this]{
        QString remark = ui->edit_remarks->text().trimmed();
        QString apply = ui->edit_apply_info->text().trimmed();
        if(remark == "" || apply =="")
            return;
        remark_ = remark;
        apply_ = apply;
        accept();
    });
    //label 编辑改变信号
    connect(ui->edit_lable,&QLineEdit::textChanged,[this](const QString &text){
        QString label_text = text.trimmed();
        if(label_text== ""){
            ui->add_label->hide();
            return;
        }
        if(!hint_label_key_.contains(label_text)){
            label_text = "添加标签: " + label_text;
        }
        ui->add_label->setText(label_text);
        ui->add_label->show();
    });

    //显示更多标签按钮信号
    connect(ui->btn_show_more,&SwitchLabel::notifyClicked,this,[=](QObject *o,bool isOpen){
        hint_label_show_all_ = isOpen;
        showHintLabel();
    });

}

void ApplyFriend::addFriendLabel(const QString &key)
{
    if(friend_labels_.contains(key))
        return;


    auto f_lb = new FriendLabel(ui->label_list_wid);
    f_lb->setText(key);
    f_lb->setProperty("class","friend_label");
    connect(f_lb,&FriendLabel::notifyClose,this,&ApplyFriend::onFriendLabelClicked);
    friend_labels_.insert(key,f_lb);
    showFriendLabel();

}

void ApplyFriend::delFriendLabel(const QString &key)
{
    auto it = friend_labels_.find(key);
    if(it == friend_labels_.end())
        return;
    it.value()->heigh();
    delete it.value();
    friend_labels_.erase(it);
    showFriendLabel();
}

void ApplyFriend::editLabelReturn()
{
    auto text = ui->edit_lable->text().trimmed();
    if(text == "")
        return;
    ui->edit_lable->clear();
    if(ui->add_label->isVisible())
        ui->add_label->hide();
    addFriendLabel(text);
    addHintLabel(text,true);
}


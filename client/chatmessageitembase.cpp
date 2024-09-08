#include "chatmessageitembase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QSpacerItem>
ChatMessageItemBase::ChatMessageItemBase(MSG_OWNER owner, QString time, QPixmap head, QWidget *content, QWidget *parent):QWidget(parent),content_(content)
  ,whether_display_time_(false)
{
    content->setParent(this);
    setAttribute(Qt::WA_StyledBackground);

    //加载头像
    labe_head_ = new QLabel(this);
    labe_head_->setFixedSize(42,42);
    labe_head_->setScaledContents(true);
    labe_head_->setPixmap(head);


    //添加时间
    auto layout_main = new QVBoxLayout(this);
    layout_main->setContentsMargins(15,5,15,5);
    layout_main->setSpacing(10);
    label_time_ = new QLabel(time);
    label_time_->setAlignment(Qt::AlignCenter);
    layout_main->addWidget(label_time_);

    //添加聊天内容界面和头像
    auto layout_content = new QHBoxLayout;
    auto spacer = new QSpacerItem(20,10,QSizePolicy::Expanding,QSizePolicy::Minimum);
    if(owner == MSG_OWNER::MYSELF){
        layout_content->setContentsMargins(0,0,5,0);
        layout_content->addSpacerItem(spacer);
        layout_content->addWidget(content);
        layout_content->addWidget(labe_head_);
    }else{
        layout_content->setContentsMargins(20,0,0,0);
        layout_content->addWidget(labe_head_);
        layout_content->addWidget(content);
        layout_content->addSpacerItem(spacer);
    }

    layout_main->addLayout(layout_content);
    label_time_->setHidden(!whether_display_time_);

}

void ChatMessageItemBase::onSideBarBtnClicked()
{

}

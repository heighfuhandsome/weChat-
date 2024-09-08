#ifndef CHATMESSAGEITEMBASE_H
#define CHATMESSAGEITEMBASE_H
#include "global.h"
#include <QObject>
#include <QLabel>
#include <QString>
class ChatMessageItemBase : public QWidget
{
    Q_OBJECT
public:
    ChatMessageItemBase(MSG_OWNER owner,QString time,QPixmap head,QWidget *content,QWidget* parent = nullptr);

    void onSideBarBtnClicked();
private:
    QLabel *label_time_;
    QLabel *labe_head_;
    QWidget *content_;
    bool whether_display_time_;
};

#endif // CHATMESSAGEITEMBASE_H

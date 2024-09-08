#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QDialog>
#include <QVector>
#include <QQueue>
#include <functional>
#include "customerscrollwidget.h"
#include "applyfriend.h"
#include "chatcontentmsg.h"
#include "global.h"
#include "searchwid.h"
#include "userinfodialog.h"
namespace Ui {
class ChatWidget;
}

class ChatWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    void initData();
    ~ChatWidget();
protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
    void closeEvent(QCloseEvent *e) override;
public slots:
    void OnSideBarRadionButtonSelected(QObject *o);
    void onSearchTextChange(QString text);
    void onUserInfoDialogExit(bool accept);
    void onWhetherAcceptAddFriendBtn(QObject *obj,bool isAccept);
    void onNetWorkCallBack(const QByteArray &);
    void switchChatpage(QObject *obj);  //跳转到聊天界面
    void onFriendInfoCome(Global::FriendInfo &info);
    void onFriendMsgCome(Global::Msg &msg);
signals:
    //保存头像
    void sendHeadImg(REQ_ID id,const QByteArray &);
    //获取头像
    void getHeadImg(REQ_ID id,const QByteArray &);
    void closed();
    //查找用户
    void searchUserByAccount(REQ_ID ,const QByteArray &);
    //申请添加好友
    void requestAddFriend(REQ_ID ,const QByteArray &);
    //获取好友申请列表
    void getApplyFriendList(REQ_ID ,const QByteArray &);
    //通过id获取用户信息
    void searchUserById(REQ_ID ,const QByteArray &);
    //是否同意好友申请
    void whetherAcceptAddFriend(REQ_ID ,const QByteArray &);
    //获取好友列表
    void getFriendList(REQ_ID ,const QByteArray &);
    //获取好友信息
    void getFriendMsg(REQ_ID ,const QByteArray &);
private:
    void regCallBack(); //注册tcp回调
    void initUi();
    void initButton();
    void initSignal();
    void switchListBar();
    void cleanJson(QJsonObject &json,QStringList);


    Ui::ChatWidget *ui;
    QVector<QObject*> navigation_radiobutton_;
    bool isSearchMode_;
    ApplyFriend *addFrienDiglog_;
    CustomerScrollWidget *friendlsit_;
    CustomerScrollWidget *friendApplyList_;
    ChatContentMsg *chatContentWid_;
    CustomerScrollWidget *msgListWid_;
    SearchWid *searchWid_;
    QQueue<QObject*> headWidgets_; //用来异步加载头像
    QPixmap head_; //头像
    UserInfoDialog *userInfoDialog_;
    QQueue<std::function<void(const QJsonObject &json)>> callbacks_;
};

#endif // CHATWIDGET_H

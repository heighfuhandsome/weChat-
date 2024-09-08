#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include <QVector>


enum class REQ_ID{
    USER_REGISTER = 1001,
    USER_LOGIN = 1002,
    USER_LOGIN_CHAT_SERVER = 1003,
    USER_SET_HEAD = 1004,
    GET_USER_HEAD = 1005 ,             //获取用户头像
    GET_USERINFO_BY_ACCOUNT = 1006, //获取用户信息通过账号
    REQUEST_ADD_FRIEND = 1007, //申请添加好友
    GET_APPLY_FRIEND_LIST = 1008, //获取申请好友列表
    GET_USERINFO_BY_ID = 1009, //获取申请好友列表
    WHETHER_ACCEPT_ADD_FRIEND = 1010, //是否接收好友添加申请
    GET_FRIEND_LIST = 1011, //获取好友列表
    SEND_MSG = 1012, //发送信息
    GET_FRIEND_MSG = 1013 //获取好友消息消息
};

enum class MSG_OWNER{
    MYSELF,
    OTHERS
};

//用户http发送管理模块回调时通知哪一个模块
enum class MODULE_ID{
    REG_MODULE,
    LOGIN_MODULE
};

enum class ERR_CODE{
    SUCCESS,
    NETWORK_ERR,
    JSON_PARSE_ERR
};


//聊天类型  决定MessageItemBase的布局显示
enum class MSG_TYPE
{
    APPLY_FRIEND,
    CHAT_MSG
};


class Global : public QObject
{
    Q_OBJECT
public:
    typedef struct{
        QString text_;
        QString time_;
        QString oppoSideId_;
        MSG_OWNER owner_;
        QObject *wid_;
        unsigned long id_;
    }Msg;

    typedef struct{
        QString remark_;
        QObject *wid_;  //他所绑定的窗口
        QString id_;
    }FriendInfo;

    explicit Global(QObject *parent = nullptr);
    void appendMsg(Msg &msg);
    inline QVector<Msg>& getMsg(QString id) { return messages_[id]; }
    static void repolish(QWidget *);
    static Global& getInstance();
private:
    QMap<QString,QVector<Msg>> messages_;
    QMap<QString,QVector<FriendInfo>> friends_;
    QMap<QString,int> skipPages_;

    void onFriendListCome(const QByteArray &data);
    void onFriendMsgCome(const QByteArray &data);
    // void getFriendMsgFromServer(QString fid);

    static unsigned long msgId;

signals:
    void notifyMsg(Msg &);
    void notifyFriend(FriendInfo &);
};

#endif // GLOBAL_H

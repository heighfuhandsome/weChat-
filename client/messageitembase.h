#ifndef MESSAGEITEMBASE_H
#define MESSAGEITEMBASE_H

#include <QWidget>
#include <QLabel>
#include "global.h"
namespace Ui {
class MessageItemBase;
}

class MessageItemBase : public QWidget
{
    Q_OBJECT

public:
    explicit MessageItemBase(QWidget *parent = nullptr);
    ~MessageItemBase();
    void setMsgType(MSG_TYPE type);
    void setMsg(const QString &msg);
    void setRemark(const QString &msg);
    void setTime(const QString &msg);
    void setHint(const QString &hint);
    void setHead(const QString &head);
    inline void setId(const QString &id){ id_ = id; }
    inline void setAccount(const QString &account){ account_ = account; }
    inline QString id(){ return id_; }
    inline QString account(){ return account_; }
    QLabel* getHead();
    QString remark();
protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
signals:
    void clicked(QObject *wid,bool isAccept);
    void clicked(QObject *wid);
private:
    void initSignal();

    Ui::MessageItemBase *ui;
    MSG_TYPE msg_type_;
    QString id_;
    QString account_;
};

#endif // MESSAGEITEMBASE_H

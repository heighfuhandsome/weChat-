#ifndef CHATCONTENTMSG_H
#define CHATCONTENTMSG_H

#include <QWidget>
#include <QMap>
#include <QVector>
#include "global.h"
namespace Ui {
class ChatContentMsg;
}

class ChatContentMsg : public QWidget
{
    Q_OBJECT

public:
    explicit ChatContentMsg(QWidget *parent = nullptr);
    ~ChatContentMsg();
    void setHead(const QPixmap &pixmap){ head_ = pixmap; }
    void setOppoSideHead(const QPixmap &pixmap){ opposide_head_ = pixmap; }

signals:
    void sendMsgToNetWork(REQ_ID,const QByteArray &);
public slots:
    void switchUser(QObject *user);
    void onMessageCome(Global::Msg &msg);

protected:
    virtual bool eventFilter(QObject *obj,QEvent *e) override;

private:
    void initStyle();
    void sendMsg();
    void flushMsgInterface(QString oldId);

    Ui::ChatContentMsg *ui;
    QString id_;
    QString title_;
    QPixmap head_;
    QPixmap opposide_head_;
};

#endif // CHATCONTENTMSG_H

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "global.h"
#include "chatwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void switchChatWidget();
    void initSignal();
    void initUi();
    ~Widget();
protected:
    virtual void mousePressEvent(QMouseEvent *e) override;

signals:
    void loginChatServer(REQ_ID id,const QByteArray &data);

private slots:
    void on_btn_login_clicked();
    void handlerResponse(REQ_ID rid,ERR_CODE error,QByteArray data);
    void handlerRegiserClose(QObject *regiserWindow);
    void updateLoginBtnState();

private:
    void showTip(QString info,bool error);
    void switchChatWid();

    bool accountIsNull_;
    bool pwdIsNull_;
    ChatWidget *chat_widget_;
    Ui::Widget *ui;
};
#endif // WIDGET_H

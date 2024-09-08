#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QTimer>
#include "global.h"
#include "httpaccessmanager.h"
namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();
protected:
    virtual void closeEvent(QCloseEvent *e) override;

signals:
    void closed(QObject *e);
private:
    bool accountIsValid_;
    bool pwdIsValid_;
    bool nickNameIsValid_;
    void updateBtnRegState();
    void initUi();
    void initSignal();
    void updateHint(QString hint, bool isError);
    void onRegister(); //槽函数
    void HandleHttpRes(REQ_ID rid,ERR_CODE ec, QByteArray data); //槽函数
    void onTimerout();
    void startTimer();

    QTimer *timer_;
    Ui::Register *ui;
};


#endif // REGISTER_H

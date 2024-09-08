#ifndef USERINFODIALOG_H
#define USERINFODIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QLabel>
namespace Ui {
class UserInfoDialog;
}

class UserInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserInfoDialog(QWidget *parent = nullptr);
    void setHeadImg(const QByteArray &data);
    void setHeadImg(const QPixmap &pixmap);
    void setNick(const QString &nick);
    void setButtonText(const QString &text);
    inline void setId(QString id){ id_ = id; };
    inline QString id(){ return id_; }
    QLabel* Head();
    ~UserInfoDialog();
protected:
    virtual void mousePressEvent(QMouseEvent *e);
signals:
    void exit(bool accept);
private:
    Ui::UserInfoDialog *ui;
    QString id_;
};

#endif // USERINFODIALOG_H

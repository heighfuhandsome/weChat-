#ifndef FRIENDLABEL_H
#define FRIENDLABEL_H

#include <QWidget>
#include <QString>

namespace Ui {
class FriendLabel;
}

class FriendLabel : public QWidget
{
    Q_OBJECT

public:
    explicit FriendLabel(QWidget *parent = nullptr);
    ~FriendLabel();
    void setText(const QString &text);
    QString text();
    int width();
    int heigh();
signals:
    void notifyClose(QObject *obj);
private:
    void initSignal();
    int width_;
    int height_;
    QString text_;

    Ui::FriendLabel *ui;
};

#endif // FRIENDLABEL_H

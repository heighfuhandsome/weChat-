#ifndef APPLYFRIEND_H
#define APPLYFRIEND_H

#include <QDialog>
#include <QPoint>
#include <QVector>
#include <QMap>
#include "friendlabel.h"
#include "switchlabel.h"
namespace Ui {
class ApplyFriend;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    inline QString remark(){ return remark_; };
    inline QString apply(){ return apply_; }
    void clean();
    void setApplyVisible(bool visible);
    ~ApplyFriend();
private slots:
    virtual void onHintLabelClick(QObject *o,bool isOpen,QString text);
    virtual void onFriendLabelClicked(QObject *obj);
protected:
    virtual bool eventFilter(QObject *o,QEvent *e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    //已存在的标签
    QMap<QString,SwitchLabel*> hint_labels_;
    QVector<QString> hint_label_key_;
    bool hint_label_show_all_;

    //为新朋友设置的标签
    QMap<QString,FriendLabel*> friend_labels_;
    QString remark_;
    QString apply_;


    void initData();
    void initStyle();
    void initUi();
    void initSignal();
    void initHintLabel();
    void showHintLabel();
    void showFriendLabel();
    void addHintLabel(const QString &key,bool isOpen);
    void addFriendLabel(const QString &key);
    void delFriendLabel(const QString &key);
    void editLabelReturn();
    Ui::ApplyFriend *ui;
};

#endif // APPLYFRIEND_H

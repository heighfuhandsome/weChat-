#ifndef FRIENDITEMBASE_H
#define FRIENDITEMBASE_H

#include <QWidget>
#include <QLabel>
namespace Ui {
class FriendItemBase;
}

class FriendItemBase : public QWidget
{
    Q_OBJECT

public:
    explicit FriendItemBase(QWidget *parent = nullptr);
    void showReadPoint(bool visible);

    inline void setId(const QString &id){ id_ = id; };
    void setRemakr(const QString &remark);
    void setHead(const QString &head);

    inline QString id(){ return id_;}
    inline QString remark(){ return remark_;}
    QLabel* head();
    ~FriendItemBase();
signals:
    void clicked(QObject *o);
protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
private:
    Ui::FriendItemBase *ui;
    QString id_;
    QString remark_;
};

#endif // FRIENDITEMBASE_H

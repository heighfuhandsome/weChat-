#ifndef SWITCHLABEL_H
#define SWITCHLABEL_H

#include <QLabel>

class SwitchLabel : public QLabel
{
    Q_OBJECT
public:
    SwitchLabel(QString open,QString close,QWidget *parent = nullptr);
    SwitchLabel(QWidget *parent = nullptr);
    inline bool isOpen(){ return is_open_; }
    void setState(QString open,QString close);
    void open();
    void close();
protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
signals:
    void notifyClicked(QObject *o,bool is_open,QString text);
private:
    void updateStyle();

    bool is_open_;
    QString state_open_;
    QString state_close_;
};

#endif // SWITCHLABEL_H

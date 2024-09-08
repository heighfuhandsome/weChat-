#ifndef CUSTOMERPUSHBUTTON_H
#define CUSTOMERPUSHBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QMouseEvent>
#include <QPixmap>

class CustomerPushButton : public QPushButton
{
    Q_OBJECT
public:
    void setImages(QString normal,QString hover,QString press);
    CustomerPushButton(QWidget *obj);
protected:
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
private:
    bool show_red_point_;
    QPixmap pixmap_;
    QString normal_;
    QString hover_;
    QString press_;
};

#endif // CUSTOMERPUSHBUTTON_H

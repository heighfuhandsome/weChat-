#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QWidget>
#include <QPixmap>

class RadioButton: public QWidget
{
    Q_OBJECT
public:
    explicit RadioButton(QWidget *parent = nullptr);
    void setImages(QString normal,QString selected);
    void setSelected(bool selected);
    inline bool isSelect(){ return is_selected_; }
protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void paintEvent(QPaintEvent *e) override;

signals:
    void notifySelected(QObject *obj);
private:
    QString normal_;
    QString selected_;
    bool is_selected_;
    QPixmap icon_;

};

#endif // SWITCHBUTTON_H

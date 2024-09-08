#ifndef CUSTOMERSCROLLWIDGET_H
#define CUSTOMERSCROLLWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QSet>
#include <QMap>
class CustomerScrollWidget : public QWidget
{
    Q_OBJECT
public:
    CustomerScrollWidget(QWidget *parent = nullptr);
    void appendWidget(QString id,QWidget *w);
    void removeWidget(QWidget *w);
    void removeWidget(QString id);
    void setEnanleAutoScore();
    inline QWidget* findWidget(QString id){return wids_[id]; }
    inline bool containId(QString id){return wids_.contains(id);}
protected:
    virtual bool eventFilter(QObject *obj,QEvent *e) override;
private:
    QScrollArea *scroll_area_;
    QVBoxLayout *layout_content_;
    QWidget *widget_content_;
    bool auto_scroll_;
    QMap<QString,QWidget*> wids_;
};

#endif // CUSTOMERSCROLLWIDGET_H

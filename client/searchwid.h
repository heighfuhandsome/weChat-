#ifndef SEARCHWID_H
#define SEARCHWID_H

#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>
namespace Ui {
class SearchWid;
}

class SearchWid : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWid(QWidget *parent = nullptr);
    void setHintVisible(bool visible);
    void setAddFrinedVisible(bool visible);
    ~SearchWid();
protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
signals:
    void searchUserClicked();
private:
    void initUi();

    Ui::SearchWid *ui;
    QPixmap icon_;
};

#endif // SEARCHWID_H

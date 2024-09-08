#include "userinfodialog.h"
#include "ui_userinfodialog.h"
#include <QPixmap>

UserInfoDialog::UserInfoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserInfoDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);

    ui->widget->layout()->setAlignment(ui->exit,Qt::AlignTop);
    ui->head->setScaledContents(true);


    connect(ui->pushButton,&QPushButton::clicked,this,[this]{
        emit exit(true);
        close();
    });
}

void UserInfoDialog::setHeadImg(const QByteArray &data)
{
    ui->head->setPixmap(QPixmap(data));
}

void UserInfoDialog::setHeadImg(const QPixmap &pixmap)
{
    ui->head->setPixmap(pixmap);
}

void UserInfoDialog::setNick(const QString &nick)
{
    ui->nikc->setText(nick);
}

void UserInfoDialog::setButtonText(const QString &text)
{
    ui->pushButton->setText(text);
}

QLabel *UserInfoDialog::Head()
{
    return ui->head;
}

UserInfoDialog::~UserInfoDialog()
{
    delete ui;
}

void UserInfoDialog::mousePressEvent(QMouseEvent *e)
{
    auto g_pos = mapToGlobal(e->pos());
    if(ui->exit->rect().contains(ui->exit->mapFromGlobal(g_pos))){
        emit exit(false);
        close();
        return;
    }
    QDialog::mousePressEvent(e);
}

#ifndef HTTPACCESSMANAGER_H
#define HTTPACCESSMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include "global.h"
class HttpAccessManager : public QObject
{
    Q_DISABLE_COPY(HttpAccessManager);
    Q_OBJECT
public:
    static HttpAccessManager& GetInstance(){
        static HttpAccessManager manager;
        return manager;
    }

    void post(REQ_ID reqId,MODULE_ID modId,QJsonDocument json);

protected:
    explicit HttpAccessManager(QObject *parent = nullptr);

signals:
    //通知注册模块收到服务端数据
    void notifyRegModule(REQ_ID rid,ERR_CODE ec,QByteArray data);
    void notifyLoginModule(REQ_ID rid,ERR_CODE ec,QByteArray data);


private:
    //槽函数
    void onFinished(QNetworkReply *reply);
    void setUrl(QNetworkRequest &req, REQ_ID id);

    QNetworkAccessManager manager_;

};

#endif // HTTPACCESSMANAGER_H

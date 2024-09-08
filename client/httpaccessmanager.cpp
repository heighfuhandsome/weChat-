#include "httpaccessmanager.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

void HttpAccessManager::post(REQ_ID reqId, MODULE_ID modId, QJsonDocument jsondoc)
{
    QNetworkRequest req;
    const QByteArray &json = jsondoc.toJson();
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json;charset=utf8;");
    req.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(json.length())); 
    setUrl(req,reqId);
    auto reply = manager_.post(req,json);
    connect(reply,&QNetworkReply::finished,this,[=]
    {
        ERR_CODE ec;
        if(reply->error() == QNetworkReply::NoError)
            ec = ERR_CODE::SUCCESS;
        else
            ec = ERR_CODE::NETWORK_ERR;
        switch (modId)
        {
        case MODULE_ID::REG_MODULE: emit notifyRegModule(reqId,ec,reply->readAll());break;
        case MODULE_ID::LOGIN_MODULE: emit notifyLoginModule(reqId,ec,reply->readAll());break;

        }
        reply->deleteLater();
    });

}

HttpAccessManager::HttpAccessManager(QObject *parent):QObject(parent)
{
}

void HttpAccessManager::setUrl(QNetworkRequest &req, REQ_ID id)
{
    switch (id) {
    case REQ_ID::USER_REGISTER : req.setUrl(QUrl{"http://127.0.0.1/register"});break;
    case REQ_ID::USER_LOGIN : req.setUrl(QUrl{"http://127.0.0.1/login"});break;
    default :break;
    }
}


#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QBuffer>
#include <QMap>
#include <functional>
#include <QDebug>
#include "global.h"
class TcpManager : public QObject
{
    Q_OBJECT
public:
    static TcpManager& GetInstance();
    TcpManager(const TcpManager&) = delete;
    TcpManager& operator=(const TcpManager&) = delete;
    void connect(QString host,uint16_t port);
    void registerHandler(REQ_ID id,const std::function<void(const QByteArray&)> &);
signals:
    void notifyConnected();
    void notifyDisConnected();
public slots:
    void onDataReady();
    void SlotSendData(REQ_ID id,const QByteArray &data);

private:
    explicit TcpManager(QObject *parent = nullptr);
    QString host_;
    uint16_t port_;
    QTcpSocket *socket_;
    QByteArray data_;
    bool head_received_;
    uint16_t tag_;
    uint16_t body_len_;
    QMap<REQ_ID,std::function<void(const QByteArray&)>> handler_;
    void handing(REQ_ID id,const QByteArray &data);
};


#endif // TCPMANAGER_H

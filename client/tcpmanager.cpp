#include "tcpmanager.h"
#include <QDataStream>
TcpManager &TcpManager::GetInstance()
{
    static TcpManager tcp_manager;
    return tcp_manager;
}

TcpManager::TcpManager(QObject *parent) : QObject(parent),head_received_(false)
{
   socket_ = new QTcpSocket(this);

   //信号初始化
    QObject::connect(socket_,&QTcpSocket::connected,this,[this]{
       qDebug()  << socket_->state();
        emit notifyConnected();
    });

    QObject::connect(socket_,&QTcpSocket::disconnected,this,[this]{
        emit notifyDisConnected();
    });

    QObject::connect(socket_,&QTcpSocket::readyRead,this,&TcpManager::onDataReady);
}

void TcpManager::handing(REQ_ID id, const QByteArray &data)
{
    if(handler_.find(id) == handler_.end())
    {
        qDebug() << "no math handler" << static_cast<int>(id);
        return;
    }
    handler_.find(id).value()(data);
}

void TcpManager::connect(QString host, uint16_t port)
{
    host_ = host;
    port_ = port;
    socket_->connectToHost(host,port);
}

void TcpManager::registerHandler(REQ_ID id, const std::function<void (const QByteArray &)> &func)
{
    handler_.insert(id,func);
}

void TcpManager::onDataReady(){
    data_.append(socket_->readAll());
    while (true) {
        if(!head_received_){
            if(data_.size() >= 4){
                QDataStream s(&data_,QIODevice::ReadOnly);
                s >> tag_ >> body_len_;
                data_ = data_.mid(4);
                head_received_ = true;
            }else{
                return;
            }

        }

        if(head_received_ && data_.size() >= body_len_){
            handing(static_cast<REQ_ID>(tag_),data_.mid(0,body_len_));
            data_ = data_.mid(body_len_);
            head_received_ = false;
        }else{
            return;
        }
    }
}

void TcpManager::SlotSendData(REQ_ID id, const QByteArray &data)
{
    qDebug() << "reqest " << static_cast<int>(id);
    QByteArray bytes;
    QDataStream s(&bytes,QIODevice::WriteOnly);
    s.setByteOrder(QDataStream::BigEndian);
    s << static_cast<uint16_t>(id) << static_cast<uint16_t>(data.size());
    bytes.append(data);
    socket_->write(bytes);
}

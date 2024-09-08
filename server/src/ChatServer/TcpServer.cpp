#include "TcpServer.h"
#include "Logger.h"
#include "RedisConnPool.h"
TcpServer::TcpServer(uint16_t port):acceptor_( IoContextPool::GetInstance(),{net::ip::address_v4::any(), port})
{
    start();
}

std::shared_ptr<ChatSession> TcpServer::getSession(const std::string &uuid)
{
    std::shared_ptr<ChatSession> ptr = nullptr;
    {
        std::lock_guard<std::mutex> guard{mutex_};
        auto it = session_.find(uuid);
        if(it != session_.end())
            ptr = it->second;
    }
    return ptr;
}

void TcpServer::start()
{
    auto session = std::make_shared<ChatSession>(IoContextPool::GetInstance(), 1024);
    session->setServer(this);
    session->setCloseCallBack([this](std::string uuid){
        std::lock_guard<std::mutex> guard{ mutex_ };
        auto iter = session_.find(uuid);
        session_.erase(iter);
    });
    session_.insert({session->uuid(), session});
    acceptor_.async_accept(session->socket(),session->endpoint(),[=](const boost::system::error_code &ec){
        if(ec){
            LOG_INFO("%s",ec.message().c_str())
            session_.erase(session->uuid());
        }else{
            LOG_DEBUG("connected")
            session->start();
        }
        start();
    });
}

void TcpServer::run()
{
    auto ptr = RedisConnPoll::getConn();
    auto ctx = ptr.get();
    redisReply *reply = (redisReply*)redisCommand(ctx,"subscribe %s",name_.c_str());
    freeReplyObject(reply);
    while ( redisGetReply(ctx,(void**)&reply) == REDIS_OK){
        if(reply->type == REDIS_REPLY_ARRAY && reply->elements==3){
            Json::Value value;
            std::string dest;
            json_reader.parse(reply->element[2]->str,value);
            std::string uuid = value["uuid"].asString();
            int req_id = value["reqid"].asInt();
            value.removeMember("uuid");
            value.removeMember("reqid");
            dest = json_writer.write(value);
            auto ptr = getSession(uuid);
            if(ptr){
                ptr->sendData(static_cast<REQ_ID>(req_id),dest.c_str(),dest.size());
            }
        }
        freeReplyObject(reply);
    }
}

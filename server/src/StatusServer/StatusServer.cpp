#include "StatusServer.h"
#include "Logger.h"
#include "Config.h"
StatusServerImpl::StatusServerImpl()
{

    ChatServer_t server;
    server.count_ = 0;
    server.name_ = "ChatServer1";
    server.host_ = Configuration::get("ChatServer1.host");
    server.port_ = Configuration::get("ChatServer1.port");
    servers_.push_back(server);

    server.name_ = "ChatServer2";
    server.host_ = Configuration::get("ChatServer2.host");
    server.port_ = Configuration::get("ChatServer2.port");
    servers_.push_back(server);
}

::grpc::Status StatusServerImpl::GetChartServer(::grpc::ServerContext *context, const ::Server::UserInfo *request, ::Server::ChatServerInfo *response)
{
    {
        std::lock_guard<std::mutex> guard{session_mutex_};
        if(session_.find(request->uid()) != session_.end()){
            session_.erase(request->uid());
        }
    }

    ChatServer_t server = servers_[0];
    {
        std::lock_guard<std::mutex> guard{ servers_mutex_ };
        std::for_each(servers_.begin(),servers_.end(),[&](const ChatServer_t &s){
            if(s.count_ < server.count_){
                server = s;
            }
            LOG_DEBUG("%d %s",s.count_,s.port_.c_str())
        });
    }
    LOG_DEBUG("%s %s",server.host_.c_str(),server.port_.c_str())
    auto token = generate_unique_string();
    response->set_host(server.host_);
    response->set_port(server.port_);
    response->set_token(token);
    insertToken(request->uid(),token);
    LOG_DEBUG("uid : %s, token : %s\n",request->uid().c_str(), token.c_str())
    return grpc::Status::OK;
}

::grpc::Status StatusServerImpl::Login(::grpc::ServerContext *context, const ::Server::LoginReq *request, ::Server::LoginRsp *response)
{
    LOG_DEBUG("login")
    bool tag;
    {
        std::lock_guard<std::mutex> guard{ session_mutex_ };
        auto iter = session_.find(request->user().uid());
        tag = iter!= session_.end() && iter->second == request->token(); 
    }

    if(!tag){
        LOG_INFO("%s","aaa")
        return grpc::Status::CANCELLED;
    }

    response->mutable_user()->set_uid(request->user().uid());
    response->set_token(request->token());
    {
        std::lock_guard<std::mutex> guard{ servers_mutex_ };
        std::for_each(servers_.begin(),servers_.end(),[=](ChatServer_t &s){
            if(s.name_ == request->host()){
                s.count_++;
                LOG_INFO("%s",s.name_.c_str()) 
            }
        });
    }
    return grpc::Status::OK;
}

void StatusServerImpl::insertToken(const std::string &uid, const std::string token)
{
    std::lock_guard<std::mutex> guard{ session_mutex_ };
    session_.insert(std::make_pair(uid,token));
}

std::string StatusServerImpl::generate_unique_string() const
{
    boost::uuids::random_generator generator;
    boost::uuids::uuid unique_id = generator();
    return boost::uuids::to_string(unique_id);
}


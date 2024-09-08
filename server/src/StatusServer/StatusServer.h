#pragma once
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_context.h>
#include <grpcpp/server_builder.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Server.grpc.pb.h"




typedef struct{
    std::string name_;
    std::string host_;
    std::string port_;
    uint32_t count_;
}ChatServer_t;

class StatusServerImpl final: public Server::StatusServer::Service{
public:
    StatusServerImpl();
    virtual ::grpc::Status GetChartServer(::grpc::ServerContext* context, const ::Server::UserInfo* request, ::Server::ChatServerInfo* response);
    virtual ::grpc::Status Login(::grpc::ServerContext* context, const ::Server::LoginReq* request, ::Server::LoginRsp* response);
private:
    void insertToken(const std::string &uid,const std::string token);
    std::string generate_unique_string() const;
private:
    std::mutex servers_mutex_;
    std::vector<ChatServer_t> servers_;    

    std::mutex session_mutex_;
    std::unordered_map<std::string,std::string> session_;
};
#pragma once
#include "noncopyable.h"
#include "Server.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <memory>
#include <queue>
#include <thread>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <unordered_map>
#include <json/json.h>
#include <mutex>

class StatusServerClient: public noncopyable{
public:
    static bool GetChatServer(std::string uid,Json::Value &rsp);
    static bool Login(std::string uid,std::string token,std::string host,Json::Value &rsp);
private:    
    static std::unique_ptr<Server::StatusServer::Stub> GetStube();
    static void RetuenStube(std::unique_ptr<Server::StatusServer::Stub> stub);

    static std::unique_ptr<StatusServerClient> client;
    static void initClient();
    StatusServerClient(uint8_t size_ = std::thread::hardware_concurrency());
    std::queue<std::unique_ptr<Server::StatusServer::Stub>> stubs_;
    std::mutex mutex_;
};
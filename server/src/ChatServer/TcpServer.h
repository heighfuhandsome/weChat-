#pragma once
#include "global.h"
#include "noncopyable.h"
#include "IoContextPool.h"
#include "ChatSession.h"
#include <unordered_map>
#include <string>
#include <mutex>
class TcpServer: public noncopyable{
public:
    TcpServer(uint16_t port);
    std::shared_ptr<ChatSession> getSession(const std::string &uuid);
    void SetName(std::string name){ name_ = name; }
    std::string Name(){ return name_;}
    void run();
private:
    void start();

    net::ip::tcp::acceptor acceptor_;
    std::mutex mutex_;
    std::unordered_map<std::string,std::shared_ptr<ChatSession>> session_;
    std::string name_;
};
#pragma once
#include "global.h"
#include "noncopyable.h"
#include "NetWorkBuffer.h"
#include <functional>
#include <boost/system/error_code.hpp>
#include <mutex>

class TcpServer;

class ChatSession : public noncopyable{
    friend class LogicSystem;
public:
    ChatSession(net::io_context &ctx,size_t tmp_size);
    ~ChatSession();
    inline net::ip::tcp::socket& socket() { return socket_;}
    inline net::ip::tcp::endpoint& endpoint() { return endpoint_; };
    inline std::string uuid(){ return uuid_;}
    inline void setCloseCallBack(const std::function<void(std::string)> &func){  close_callBack_ = func;  };
    inline void setUid(std::string uid){ uid_ = uid;}
    inline void setToken(std::string token){ token_ =token;  } 
    void sendData(REQ_ID id,const char* data,size_t len);
    inline void setServer(TcpServer *server){ server_ = server; }


    void start();
private:
    void handlingRaad(const boost::system::error_code &ec,std::size_t bytes_transferred);
    void handlingWrite(const boost::system::error_code &ec,std::size_t bytes_transferred);
    bool opposideInfo(std::string &host,std::string &uuid); //判断接收方是否在线
    bool forwardMsg(REQ_ID id,const std::string &msg);
    void close();
private:
    net::ip::tcp::socket socket_;
    net::ip::tcp::endpoint endpoint_;

    std::string uid_; //客户端id
    std::string uuid_; //ChatSession 唯一id
    std::string token_; 
    bool isLogin_; //是否已经登录
    std::function<void(std::string)> close_callBack_;
    uint16_t tag_;
    uint16_t body_size_;
    bool head_parsed_;
    struct block{
        char* operator*(){ return data_; }
        char* data_;
        int len_;
    }block_;
    NetWorkBuffer buffer_;
    NetWorkBuffer write_buffer_;
    bool be_sending_;
    TcpServer *server_;
    std::string forwardId_;
    std::mutex mutex_;
};
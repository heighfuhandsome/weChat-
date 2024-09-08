#pragma once
#include "global.h"
#include <memory> 
#include <iostream>

class HttpSession: public std::enable_shared_from_this<HttpSession>{
    friend class LogicSystem;
public:
    HttpSession(net::io_context &ioc);
    // ~HttpSession(){ std::cout << "session destroy\n"; };
    ~HttpSession(){};
    void start();
    tcp::socket& socket(){ return socket_; }
private:
    void handerRequest();
    void checkDeadline();
    void writeResponse(bool flag);
    void close();

    tcp::socket socket_;
    net::steady_timer timer_;
    beast::flat_buffer buffer_{8192};
    http::request<http::dynamic_body> request_;
    http::response<http::dynamic_body> response_;
};
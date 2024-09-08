#pragma once
#include "global.h"


class HttpServer{

public:
    HttpServer(net::io_context &ioc, uint16_t port);

private:
    void start();
    tcp::acceptor acceptor_;
    net::io_context& ioc_;
};
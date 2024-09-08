#include "HttpServer.h"
#include "HttpSession.h"
#include "IoContextPool.h"
#include <iostream>
HttpServer::HttpServer(net::io_context &ioc, uint16_t port):ioc_(ioc),acceptor_(ioc,tcp::endpoint{tcp::v4(), port})
{
    start();
}

void HttpServer::start()
{
    auto ptr = std::make_shared<HttpSession>(IoContextPool::GetInstance());
    try
    {
        acceptor_.async_accept(ptr->socket(),[=](beast::error_code ec){
            if(!ec){
                ptr->start();
            }else{
                std::cout << ec.message();
            }
            start(); 
        });
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

#include "HttpSession.h"
#include "LogicSystem.h"
#include "Logger.h"
#include <iostream>

HttpSession::HttpSession(net::io_context &ioc):socket_(ioc),timer_(ioc,net::chrono::seconds(10)) {
}

void HttpSession::start()
{
    http::async_read(socket_, buffer_, request_, [self = shared_from_this()](beast::error_code ec, std::size_t btyes_transferred){
        boost::ignore_unused(btyes_transferred);
        if(ec){
            LOG_INFO("%s\n",ec.message().c_str());
            self->close();
            return;
        }
        self->handerRequest();
        self->checkDeadline();
    });
}

void HttpSession::handerRequest()
{
    writeResponse(LogicSystem::getInstance().handleRequest(shared_from_this()));
}

void HttpSession::checkDeadline()
{
        
    timer_.async_wait([self=shared_from_this()](beast::error_code ec){
        if (!ec)
        {
            self->close();
        }
    });
}

void HttpSession::writeResponse(bool flag)
{
    response_.version(request_.version());
    response_.set(http::field::keep_alive,"false");
    response_.set(http::field::host,"GateWay");

    //解析失败
    if(!flag){
        response_.result(http::status::not_found);
        beast::ostream(response_.body()) << u8"<h1>file not find</h1>";
        response_.set(http::field::content_type,"text/html; charset=UTF-8");
    }else{
        response_.result(http::status::ok);
    }

    response_.content_length(response_.body().size());
    response_.prepare_payload();

    http::async_write(socket_,response_,[self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred){
        boost::ignore_unused(bytes_transferred);
        if(ec)
        {
            LOG_INFO("%s\n", ec.message().c_str());
            self->close();
            return;
        }
        self->timer_.cancel();
    });
    

}

void HttpSession::close()
{
    boost::system::error_code ec;
    socket_.shutdown(net::socket_base::shutdown_both,ec); 
    if(ec){
        LOG_INFO("%s\n", ec.message().c_str());
    }
    socket_.close(ec);
    if(ec){
        LOG_INFO("%s\n", ec.message().c_str());
    }
}

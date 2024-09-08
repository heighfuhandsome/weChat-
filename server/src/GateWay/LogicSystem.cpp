#include "LogicSystem.h"
#include "HttpSession.h"
#include "MysqlDao.h"
#include "StatusServerClient.h"
#include <boost/url.hpp>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include <exception>
thread_local Json::Reader jsonReader;
thread_local Json::FastWriter jsonWriter;

LogicSystem& LogicSystem::getInstance(){
    static LogicSystem system;
    return system;
}

bool LogicSystem::handleRequest(std::shared_ptr<HttpSession> session)
{
    std::string url_str;
    if(!urlDecode(session->request_.target(),url_str))
    {
        return false;
    }

    boost::url_view url(url_str);
    if(session->request_.method() == http::verb::get)
    {
        auto handler = getHandlerMap_.find(url.path());
        if(handler == getHandlerMap_.end())
            return false;

        std::unordered_map<std::string,std::string> params;
        for(auto it = url.params().begin(); it!=url.params().end();it++)
        {
            params.insert({(*it).key,(*it).value});
        }
        return handler->second(session,std::move(params));
    }
    else
    {

        auto handler = postHandlerMap_.find(url.path());
        if(handler == postHandlerMap_.end())
            return false;
        else
            return handler->second(session, beast::buffers_to_string(session->request_.body().data()));
    }

    return false;
}

LogicSystem::LogicSystem()
{

    getHandlerMap_.insert({"/get_test",[](std::shared_ptr<HttpSession> session,std::unordered_map<std::string,std::string> params) -> bool{  
        beast::ostream(session->response_.body()) << "<h1>hello world</h1>";
        for(auto it = params.begin();it!=params.end();it++)
        {
            beast::ostream(session->response_.body()) << "<h3>" << it->first << " : "  << it->second << "</h3>";
        }

        session->request_.set(http::field::content_type,"text/html;charset=utf-8;");
        return true;
    }});


    //注册
    postHandlerMap_.insert({"/register", [&](std::shared_ptr<HttpSession> session,const std::string &resBody){
        Json::Value src;
        Json::Value dest;
        try{
            jsonReader.parse(resBody,src,false);
            if(src["account"].isNull() || src["nick"].isNull() || src["password"].isNull() )
            {
                dest["success"] = 0;
                dest["content"] = "缺少参数";
            }else{
               //持久化注册信息
               std::string ret; 
               bool success = MysqlDao::register_user(src["account"].asString(),src["password"].asString(),src["nick"].asString(),ret);
               dest["success"] = success? 1 : 0;
               dest["content"] = ret;
            }
        }catch(std::exception &e){
            dest["success"] = 0;
            std::cerr << e.what() << '\n';
        }

        session->response_.set(http::field::content_type,"application/json;charset=utf8;");
        beast::ostream(session->response_.body()) << jsonWriter.write(dest);
        return true;
    }});


    //登录
    postHandlerMap_.insert({"/login", [&](std::shared_ptr<HttpSession> session,const std::string &resBody){
        Json::Value src;
        Json::Value dest;
        try{
            jsonReader.parse(resBody,src,false);
            if(src["account"].isNull() || src["password"].isNull()){
                dest["success"] = 0;
                dest["content"] = "缺少参数";
            }else{
                std::string rsp;
                Json::Value rpc_rsp;
                bool verify_ok = MysqlDao::check_pwd(src["account"].asString(),src["password"].asString(),rsp);
                if(verify_ok  && StatusServerClient::GetChatServer(rsp,rpc_rsp)){
                    dest["success"] = 1;
                    dest["host"] = rpc_rsp["host"].asString();
                    dest["port"] = rpc_rsp["port"].asString();
                    dest["token"] = rpc_rsp["token"].asString();
                    dest["uid"] = rsp;
                }else{
                    dest["success"] = 0;
                    dest["content"] = rsp;
                }
            }
        }catch(std::exception &e){
            dest["success"] = 0;
            std::cerr << e.what() << '\n';
        }

        session->response_.set(http::field::content_type,"application/json;charset=utf8;");
        
        beast::ostream(session->response_.body()) << jsonWriter.write(dest);
        return true;
    }});
    
}


unsigned char FromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}


bool urlDecode(const std::string& url,std::string &dest)
{
    size_t len = url.length();
    for(int i=0;i<len ;i++)
    {
        char ch = url[i];
        if(ch != '%')
        {
            dest+=ch;
            continue;
        }

        if(ch =='%' && i+2 < len)
        {
            u_char high = FromHex(url[++i]);
            u_char low = FromHex(url[++i]);
            dest += (  (high<<4) | low );
        }
        else
        {
            return false;
        }

    }

    return true;
}



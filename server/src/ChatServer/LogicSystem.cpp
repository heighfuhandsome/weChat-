#include "LogicSystem.h"
#include "ChatSession.h"
#include "StatusServerClient.h"
#include "Logger.h"
#include "MysqlDao.h"
#include "RedisConnPool.h"
#include "TcpServer.h"
#include <json/writer.h>
#include <json/reader.h>
#include <boost/beast/core/detail/base64.hpp>
#include <string>
#include <any>

bool LogicSystem::handingReqest(ChatSession *session,REQ_ID rid,const std::string &body){
    auto it = handler_.find(rid);
    if(it == handler_.end()){
        LOG_DEBUG("未知请求%d",static_cast<int>(rid))
        return false;
    }
    return it->second(session,body); 
}


void LogicSystem::sendJson(REQ_ID rid,ChatSession *session, const Json::Value &json)
{
    auto str = json_writer.write(json);
    session->sendData(rid,str.data(),str.size());
}

bool LogicSystem::verifyFiled(const Json::Value &json, const std::initializer_list<std::string> &keys)
{
    auto begin = keys.begin();
    for(begin;begin!=keys.end();begin++){
        if(json[*begin].isNull())
            return false;
    }
    return  true;
}

bool LogicSystem::authentication(ChatSession *session, const std::string &uid, const std::string &token)
{
    LOG_INFO("验证 uid %s token %s",uid.c_str(),token.c_str())
    bool tag = session->uid_ == uid && session->token_ == token;
    if(!tag)
        LOG_DEBUG("%s","身份核实未通过")
    return tag;
}

bool LogicSystem::hadingLogin(ChatSession *session, const std::string &body)
{
    Json::Value src;
    Json::Value rsp;
    try
    {
        json_reader.parse(body,src,false);
        std::string uid;
        std::string token;
        std::string host = session->server_->Name();
        if(src["uid"].isNull() || src["token"].isNull())
            return false;
        uid = src["uid"].asString();
        token = src["token"].asString();
        LOG_DEBUG("uid : %s, token : %s",uid.c_str(), token.c_str());
        LOG_DEBUG("start login")
        if(!StatusServerClient::Login(uid,token,host,rsp))
        {
            LOG_DEBUG("login fail\n")
            return false;
        }
        session->uid_ = uid;
        session->token_ = token;
        rsp["uid"] = uid;
        rsp["token"] = token;
        Json::String rsp_str = json_writer.write(rsp);
        session->sendData(REQ_ID::USER_LOGIN_CHAT_SERVER,rsp_str.c_str(),rsp_str.size()); 
        auto redis_conn = RedisConnPoll::getConn();
        redisReply *reply;
        reply = (redisReply*)redisCommand(redis_conn.get(),"hset %s host %s",uid.c_str(), host.c_str());
        freeReplyObject(reply);
        reply = (redisReply*)redisCommand(redis_conn.get(),"hset %s uuid %s",uid.c_str(), session->uuid_.c_str());
        freeReplyObject(reply);
        session->isLogin_ = true;
        return true;
    }
    catch(const std::exception& e)
    {
       LOG_INFO("%s\n",e.what());
    }
    return false;
}

bool LogicSystem::saveHeadImage(ChatSession *session, const std::string &body)
{
    Json::Value src,dest;
    std::string uid,token,sql_rsp;
    try
    {
        bool tag = json_reader.parse(body,src,false);

        if(src["uid"].isNull() || src["token"].isNull() || src["img"].isNull()){
            dest["success"] = 0;
            dest["content"] = "missing params";
        }else{
            uid = src["uid"].asString();
            token = src["token"].asString();
            if(!authentication(session,uid,token))
                return false;

            //解码图片文件
            char img[65536]; 
            auto base64 = src["img"].asString();
            auto ret = boost::beast::detail::base64::decode(img,base64.c_str(),base64.size());
            // LOG_DEBUG("base64 out: %lu base64 in: %lu",ret.first,ret.second)
            //写入数据库
            bool tag = MysqlDao::save_user_head(uid,std::string(img,ret.first),sql_rsp);
            if(tag){
                dest["success"] = 1;
            }else{
                dest["success"] = 0;
                dest["content"] = sql_rsp;
            }
        }
        auto && rsp_str = json_writer.write(dest);
        session->sendData(REQ_ID::USER_SET_HEAD,rsp_str.c_str(),rsp_str.size());
        return true;
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
        return false;
    }

    return false;
}

bool LogicSystem::getUserHead(ChatSession *session, const std::string & body)
{
    Json::Value src,dest;
    std::string sql_rsp,uid,token;
    try
    {
        json_reader.parse(body,src,false);
        while (true){
            if(!verifyFiled(src,{"uid","token","search_id"})){
                dest["content"] = "missing params";
                dest["success"] = 0;
                break;
            }

            uid = src["uid"].asString();
            token = src["token"].asString();

            if(!authentication(session,uid,token))
                return false;
            
            if(!MysqlDao::getUserHead(src["search_id"].asString(),sql_rsp)){
                dest["success"] = 0;
                dest["content"] = sql_rsp;
                break;
            }
            // LOG_DEBUG("sql rsp size %lu",sql_rsp.size())
            char img[65535]{0};
            auto len = beast::detail::base64::encode(img,sql_rsp.c_str(),sql_rsp.size());
            dest["success"] = 1;
            dest["content"] = std::string(img,len);
            break;
        }
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
        dest["success"] = 0;
    }
    sendJson(REQ_ID::GET_USER_HEAD,session,dest);
    return true; 
}

bool LogicSystem::getUserInfoByAccount(ChatSession *session, const std::string & body)
{
    Json::Value src,dest;
    std::string sql_rsp;
    try
    {
        json_reader.parse(body,src,false);
        while (true){
            if (!verifyFiled(src, {"account","uid","token"})){
                dest["success"] = 0;
                dest["content"] = "missing params";
                break;
            }
            if(!authentication(session,src["uid"].asString(),src["token"].asString()))
                return false;

            if(!MysqlDao::getUserInfoByAccount(src["account"].asString(),sql_rsp)){
                dest["success"] = 0;
                dest["content"] = sql_rsp;
                break;
            }
            json_reader.parse(sql_rsp,src,false);
            dest["success"] = 1;
            dest["id"] = src["id"].asString();
            dest["nick"] = src["nick"].asString();
            break;
        }
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }
    sendJson(REQ_ID::GET_USERINFO_BY_ACCOUNT,session,dest);
    return true;
}

bool LogicSystem::requestAddFriend(ChatSession *session, const std::string &body)
{
    Json::Value src,dest;
    std::string sql_rsp;
    try
    {
        json_reader.parse(body,src,false);
        std::string uid,token,toid,remark,apply;
        while (true)
        {
            if(!verifyFiled(src,{"uid","token","remark","apply","toid"})){
                dest["success"] = 0;
                dest["content"] = "missing parmas"; 
                break;
            }

            uid = src["uid"].asString();
            token = src["token"].asString();
            remark = src["remark"].asString();
            apply = src["apply"].asString();
            toid = src["toid"].asString();

            if(!authentication(session,src["uid"].asString(),src["token"].asString()))
                return false;

            if(MysqlDao::requestAddFriend(uid,toid,apply,remark,sql_rsp)){
                dest["success"] = 1;
                session->forwardId_ = src["toid"].asString();
                break;
            }
            dest["success"] = 0;
            dest["content"] = sql_rsp;
            session->forwardId_ = "-1";
            break;
        }
    }
    catch(const std::exception& e)
    {
        LOG_DEBUG("%s",e.what())
    }
    sendJson(REQ_ID::REQUEST_ADD_FRIEND,session,dest);
    return true;
}

bool LogicSystem::getApplyFriendList(ChatSession * session, const std::string & body)
{
    Json::Value dest,src;
    std::string sql_rsp;
    try
    {

        while (true)
        {
            json_reader.parse(body,src,false);
            if(!verifyFiled(src,{"uid","token"})){
                dest["success"] = 0;
                dest["content"] = "缺少参数";
                break;
            }

            if(!authentication(session,src["uid"].asString(),src["token"].asString()))
                return false;

            if(MysqlDao::getApplyFriendList(src["uid"].asString(),sql_rsp)){
                dest["success"] = "1";
                json_reader.parse(sql_rsp,src,false);
                dest["content"] = src;
                break;
            }
            dest["success"] = 0;
            dest["content"] = sql_rsp;
            break;
        }
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }
    sendJson(REQ_ID::GET_APPLY_FRIEND_LIST,session,dest);
    return true;
}

bool LogicSystem::getUserInfoById(ChatSession *session, const std::string &body)
{
    Json::Value src,dest;
    std::string sql_rsp;
    try
    {
        while (true)
        {
            json_reader.parse(body, src, false);
            if (!verifyFiled(src, {"uid", "token","search_id"}))
            {
                dest["success"] = 0;
                dest["content"] = "缺少参数";
                break;
            }

            if(!authentication(session,src["uid"].asString(),src["token"].asString()))
                return false;

            if(!MysqlDao::getUserInfoById(src["search_id"].asString(),sql_rsp)){
                dest["success"] = 0;
                dest["content"] = sql_rsp;
                break;
            }

            json_reader.parse(sql_rsp,src,false);
            dest["success"] = 1;
            dest["account"] = src["account"];
            dest["nick"] = src["nick"];
            break;
        }
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what()) 
    }
    sendJson(REQ_ID::GET_USERINFO_BY_ID,session,dest);
    return true;
}

bool LogicSystem::whetherAcceptAddFriend(ChatSession *session, const std::string &body)
{
    Json::Value src,dest;
    std::string sql_rsp;
    std::string uid,token,fromid,remark;
    bool isAccept;
    try
    {
        while (true){
            json_reader.parse(body,src,false);
            if(!verifyFiled(src,{"uid","token","fromid","remark","isAccept"})){
                dest["content"] = "缺少参数";
                dest["success"] = "0";
                break;
            }

            uid = src["uid"].asString();
            token = src["token"].asString();
            fromid = src["fromid"].asString();
            remark = src["remark"].asString();
            isAccept = src["isAccept"].asBool();

            if(!authentication(session,uid,token))
                return false;
            if(!MysqlDao::whetherAcceptAddFriend(fromid,uid,remark,isAccept,sql_rsp)){
                session->forwardId_ = "-1";
                dest["success"] = 0;
            }
            else{
                if(isAccept){
                    session->forwardId_ = src["fromid"].asString();
                }else{
                    session->forwardId_ = "-1";
                }
                dest["success"] = 1;
            }
            dest["content"] = sql_rsp;
            break;
        }
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }
    sendJson(REQ_ID::WHETHER_ACCEPT_ADD_FRIEND,session,dest);
    return true;
}

bool LogicSystem::getFriendList(ChatSession *session, const std::string &body)
{
    Json::Value src,dest;
    std::any sql_rsp;
    try
    {
        while (true)
        {
            json_reader.parse(body,src,false);
            if(!verifyFiled(src,{"uid","token"})){
                dest["success"] = 0;
                dest["content"] = "缺少参数";
                break;
            }

            if(!authentication(session,src["uid"].asString(),src["token"].asString()))
                return false;

            if(!MysqlDao::getFriendList(src["uid"].asString(),sql_rsp)){
                dest["success"] = 0;
                dest["content"] = std::any_cast<std::string>(sql_rsp);
            }else{
                dest["success"] = 1;
                dest["content"] = std::any_cast<Json::Value>(sql_rsp);
            }
            break;
        }
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }
    sendJson(REQ_ID::GET_FRIEND_LIST,session,dest);
    return true; 
}

bool LogicSystem::sendMsg(ChatSession *session, const std::string &body)
{

    Json::Value src,dest;
    std::string sql_rsp;
    try
    {
        while (true)
        {
            json_reader.parse(body,src,false);
            if(!verifyFiled(src,{"uid","token","toid","text","time"})){
                dest["success"] = 0;
                dest["content"] = "缺少参数";
                break;
            }

            if(!authentication(session,src["uid"].asString(),src["token"].asString()))
                return false;

            if(!MysqlDao::sendMessage(src["uid"].asString(),src["toid"].asString(),src["text"].asString(),src["time"].asString(),sql_rsp)){
                dest["success"] = 0;
                session->forwardId_ = "-1";
            }else{
                dest["success"] = 1;
                session->forwardId_ = src["toid"].asString();
            }
            dest["content"] = sql_rsp;
            break;
        }
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }
    sendJson(REQ_ID::GET_FRIEND_LIST,session,dest);
    return true; 
}

bool LogicSystem::getFriendMsg(ChatSession *session, const std::string &body)
{
    Json::Value src,dest;
    std::any sql_rsp;
    try
    {
        while (true)
        {
            json_reader.parse(body,src,false);
            if(!verifyFiled(src,{"uid","token","fid","skipPage"})){
                dest["success"] = 0;
                dest["content"] = "缺少参数";
                break;
            }

            if(!authentication(session,src["uid"].asString(),src["token"].asString()))
                return false;

            if(!MysqlDao::getFriendMsg(src["uid"].asString(),src["fid"].asString(),src["skipPage"].asInt() ,sql_rsp)){
                dest["success"] = 0;
                dest["content"] = std::any_cast<std::string>(sql_rsp);
            }else{
                dest["success"] = 1;
                dest["content"] = std::any_cast<Json::Value>(sql_rsp);
            }
            break;
        }
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }
    sendJson(REQ_ID::GET_FRIEND_MSG,session,dest);
    return true; 

}

LogicSystem::LogicSystem(){
    handler_.insert({REQ_ID::USER_LOGIN_CHAT_SERVER,std::bind(&LogicSystem::hadingLogin,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::USER_SET_HEAD,std::bind(&LogicSystem::saveHeadImage,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::GET_USER_HEAD,std::bind(&LogicSystem::getUserHead,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::GET_USERINFO_BY_ACCOUNT,std::bind(&LogicSystem::getUserInfoByAccount,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::REQUEST_ADD_FRIEND,std::bind(&LogicSystem::requestAddFriend,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::GET_APPLY_FRIEND_LIST,std::bind(&LogicSystem::getApplyFriendList,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::GET_USERINFO_BY_ID,std::bind(&LogicSystem::getUserInfoById,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::WHETHER_ACCEPT_ADD_FRIEND,std::bind(&LogicSystem::whetherAcceptAddFriend,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::GET_FRIEND_LIST,std::bind(&LogicSystem::getFriendList,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::SEND_MSG,std::bind(&LogicSystem::sendMsg,this,std::placeholders::_1,std::placeholders::_2)});
    handler_.insert({REQ_ID::GET_FRIEND_MSG,std::bind(&LogicSystem::getFriendMsg,this,std::placeholders::_1,std::placeholders::_2)});
}
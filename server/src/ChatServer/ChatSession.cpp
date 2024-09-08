#include "ChatSession.h"
#include "Logger.h"
#include "RedisConnPool.h"
#include "LogicSystem.h"
#include "TcpServer.h"
#include "MysqlDao.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
ChatSession::ChatSession(net::io_context &ctx,size_t tmp_size):tag_(0),body_size_(0),head_parsed_(false),socket_(ctx),buffer_(tmp_size),be_sending_(false),write_buffer_(tmp_size),isLogin_(false)
{
    block_.data_ = new char[tmp_size];
    block_.len_ = tmp_size;
    auto  uuid = boost::uuids::random_generator()();
    uuid_ = boost::uuids::to_string(uuid);
}
ChatSession::~ChatSession()
{
    delete[] block_.data_;
    block_.data_ = nullptr;
}
void ChatSession::sendData(REQ_ID id, const char *data, size_t len)
{
    std::lock_guard<std::mutex> guard{mutex_};
    write_buffer_.prepareUInt16(static_cast<uint16_t>(id));
    write_buffer_.prepareUInt16(len);
    write_buffer_.write(data,len);
    if(be_sending_)
        return;
    socket_.async_write_some(net::buffer(write_buffer_.peek(),write_buffer_.readAbleBytes()),std::bind(&ChatSession::handlingWrite,this,net::placeholders::error,net::placeholders::bytes_transferred));
    be_sending_ = true;
}
void ChatSession::start()
{
    socket_.async_read_some(net::buffer(*block_,block_.len_), std::bind(&ChatSession::handlingRaad,this,net::placeholders::error,net::placeholders::bytes_transferred));
}

void ChatSession::handlingRaad(const boost::system::error_code &ec,std::size_t bytes_transferred)
{
    if(ec){
        LOG_INFO("%s",ec.message().c_str())
        close();
        return;
    }
    buffer_.write(*block_,bytes_transferred);
    
    while(true)
    {
        if(!head_parsed_ && buffer_.readAbleBytes() >= 4){
            tag_ = buffer_.revrieveUInt16();
            body_size_ = buffer_.revrieveUInt16();
            head_parsed_ = true;
            LOG_DEBUG("tag : %d,body_len : %d",tag_,body_size_);
        }

        if(head_parsed_ && buffer_.readAbleBytes() >= body_size_){
            std::string body = buffer_.retrivevAsString(body_size_);
            head_parsed_ = false;
            bool success = LogicSystem::GetInstance().handingReqest(this,static_cast<REQ_ID>(tag_),body); 
            if(!success){
                LOG_DEBUG("该请求执行未能成功，断开客户端连接 tag : %d",tag_);
                close();
                return;
            }

            //消息转发
            if(forwardId_ == "-1")
                continue;

            if(REQ_ID::SEND_MSG == static_cast<REQ_ID>(tag_)){
                forwardMsg(REQ_ID::SEND_MSG,body);
            }else if(REQ_ID::REQUEST_ADD_FRIEND == static_cast<REQ_ID>(tag_)){
                forwardMsg(REQ_ID::REQUEST_ADD_FRIEND,body);
            }else if(REQ_ID::WHETHER_ACCEPT_ADD_FRIEND == static_cast<REQ_ID>(tag_)){
                forwardMsg(REQ_ID::WHETHER_ACCEPT_ADD_FRIEND,body);
            }
        }else{
            break;
        }
    }
    start();
}

void ChatSession::handlingWrite(const boost::system::error_code &ec, std::size_t bytes_transferred)
{
    write_buffer_.retrieveN(bytes_transferred); 
    if(write_buffer_.readAbleBytes())
        socket_.async_write_some(net::buffer(write_buffer_.peek(),write_buffer_.readAbleBytes()),std::bind(&ChatSession::handlingWrite,this,net::placeholders::error,net::placeholders::bytes_transferred));
    else
        be_sending_ = false;
}


bool ChatSession::opposideInfo(std::string &host, std::string &uuid)
{
    auto redis_ctx = RedisConnPoll::getConn();
    redisReply *reply = (redisReply*)redisCommand(redis_ctx.get(),"hget %s host",forwardId_.c_str());
    if(reply->type == REDIS_REPLY_NIL){
        freeReplyObject(reply);
        return false;
    }else if(reply->type == REDIS_REPLY_STRING){
        host = reply->str;  
        freeReplyObject(reply);
    }

    reply = (redisReply*)redisCommand(redis_ctx.get(),"hget %s uuid",forwardId_.c_str());
    if(reply->type == REDIS_REPLY_NIL){
        freeReplyObject(reply);
        return false;
    }else if(reply->type == REDIS_REPLY_STRING){
        uuid = reply->str;  
        freeReplyObject(reply);
    }
    return true;
}

bool ChatSession::forwardMsg(REQ_ID id, const std::string &msg)
{
    std::string host,uuid;
    if(!opposideInfo(host,uuid))
        return false;
    
    std::string dest;
    Json::Value value;
    
    //组织数据
    if(id == REQ_ID::SEND_MSG){
        Json::Value src,array(Json::arrayValue),item;
        json_reader.parse(msg,src,false);
        item["fromid"] = uid_;
        item["toid"] = forwardId_;
        item["text"] = src["text"].asString();
        item["time"] = src["time"].asString();
        array.append(item);
        value["success"] = 1;
        value["content"] = array;

        if(host != server_->Name()){
            value["reqid"] = static_cast<int>(REQ_ID::GET_FRIEND_MSG);
            value["uuid"] = uuid;
        }
    }else if(id == REQ_ID::REQUEST_ADD_FRIEND){
        Json::Value src,array(Json::arrayValue),item;
        json_reader.parse(msg,src,false);
        item["fromid"] = uid_;
        item["applyInfo"] = src["apply"].asString();
        array.append(item);
        value["success"] = 1;
        value["content"] = array;
        if(host != server_->Name()){
            value["reqid"] = static_cast<int>(REQ_ID::GET_APPLY_FRIEND_LIST);
            value["uuid"] = uuid;
        }
    }else if(id == REQ_ID::WHETHER_ACCEPT_ADD_FRIEND){
        Json::Value array(Json::arrayValue),item;
        std::string remark;
        if(!MysqlDao::getFriendgRemark(forwardId_,uid_,remark))
            return false;
        item["id"] = uid_;
        item["remark"] = remark;
        array.append(item);
        value["success"] = 1;
        value["content"] = array;
        if(host != server_->Name()){
            value["reqid"] = static_cast<int>(REQ_ID::GET_FRIEND_LIST);
            value["uuid"] = uuid;
        }
    }else{
        return false;
    }

    dest = json_writer.write(value);
    //发送
    if(server_->Name() == host){
        auto ptr = server_->getSession(uuid);
        if(ptr==nullptr)
            return false;
        ptr->sendData(REQ_ID::GET_FRIEND_MSG,dest.c_str(),dest.size());
        return true;
    }else{
        auto redis_conn = RedisConnPoll::getConn();
        redisReply *reply;
        reply =  (redisReply*)redisCommand(redis_conn.get(),"publish %s %s",host.c_str(),dest.c_str());
        LOG_DEBUG("%d",reply->type)
        freeReplyObject(reply);
        return true;
    }
}


void ChatSession::close()
{
    if(isLogin_){
        auto redis_conn = RedisConnPoll::getConn();
        redisReply *reply = (redisReply*)redisCommand(redis_conn.get(),"del %s",uid_.c_str());
        freeReplyObject(reply);
        isLogin_ = false;
    }

    boost::system::error_code ec;
    socket_.shutdown(net::socket_base::shutdown_both, ec);
    if (ec)
        LOG_INFO("%s", ec.message().c_str())
    if (socket_.is_open())
    {
        socket_.close(ec);
        if (ec)
            LOG_INFO("%s", ec.message())
    }
    if (close_callBack_)
        close_callBack_(uuid_);
}

#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <json/reader.h>
#include <json/writer.h>
#include <json/json.h>

static thread_local Json::FastWriter json_writer;
static thread_local Json::Reader json_reader;
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


enum class REQ_ID{
    USER_REGISTER = 1001,
    USER_LOGIN = 1002,
    USER_LOGIN_CHAT_SERVER = 1003,
    USER_SET_HEAD = 1004,          //设置头像图片
    GET_USER_HEAD  = 1005, //获取用户头像
    GET_USERINFO_BY_ACCOUNT = 1006 ,//获取用户信息通过账号
    REQUEST_ADD_FRIEND  = 1007, //请求添加好友
    GET_APPLY_FRIEND_LIST = 1008, //获取申请好友列表
    GET_USERINFO_BY_ID = 1009, //通过id查询用户信息
    WHETHER_ACCEPT_ADD_FRIEND = 1010,//是否接收好友添加申请
    GET_FRIEND_LIST = 1011, //获取好友列表
    SEND_MSG = 1012, //发送信息
    GET_FRIEND_MSG = 1013 //获取好友消息
};


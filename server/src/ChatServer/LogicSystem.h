#pragma once
#include "Singleton.h"
#include "global.h"
#include <json/json.h>
#include <unordered_map>
#include <functional>
#include <string>
#include <initializer_list>
class ChatSession;
class LogicSystem : public Singleton<LogicSystem>{
    friend class Singleton<LogicSystem>;

public:
    LogicSystem();
    bool handingReqest(ChatSession *ssion,REQ_ID rid,const std::string &body);

private:
    bool authentication(ChatSession *session,const std::string &uid,const std::string &token);
    bool hadingLogin(ChatSession*,const std::string &body);
    bool saveHeadImage(ChatSession*,const std::string &body); //保存头像
    bool getUserHead(ChatSession*,const std::string &body); //获取用户头像
    bool getUserInfoByAccount(ChatSession*,const std::string &body); //获取用户信息通过账号
    bool requestAddFriend(ChatSession*session,const std::string &body); //用户请求添加好友
    bool getApplyFriendList(ChatSession*session,const std::string &body); //获取好友申请列表
    bool getUserInfoById(ChatSession*session,const std::string &body); //通过id查找用户信息
    bool whetherAcceptAddFriend(ChatSession*session,const std::string &body); // 是否同意好友申请
    bool getFriendList(ChatSession*session,const std::string &body); // 获取好友列表
    bool sendMsg(ChatSession*session,const std::string &body); // 发送消息
    bool getFriendMsg(ChatSession*session,const std::string &body); // 获取消息列表


    void sendJson(REQ_ID rid,ChatSession *session,const Json::Value &json);
    bool verifyFiled(const Json::Value &json,const std::initializer_list<std::string> &keys); //检查是否缺少字段
    std::unordered_map<REQ_ID,std::function<bool(ChatSession*,const std::string &body)>>  handler_;
};

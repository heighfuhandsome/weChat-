#pragma once
#include "noncopyable.h"
#include "MysqlConnectPool.h"
#include "Logger.h"
#include <any>
class MysqlDao : public noncopyable{

public:
    static bool register_user(const std::string &account,const std::string &pwd,const std::string &nick,std::string &rsp);
    static bool check_pwd(const std::string &account,const std::string &pwd,std::string &rsp);
    static bool save_user_head(const std::string &uid,const std::string &img,std::string &rsp);       //保存用户头像
    static bool getUserHead(const std::string &uid,std::string &rsp);       //获取用户头像
    static bool getUserInfoByAccount(const std::string &account,std::string &rsp);       //通过账号查找用户
    static bool requestAddFriend(const std::string &uid, const std::string &toid,const std::string &apply,const std::string  &remark,std::string &rsp);       //申请添加好友
    static bool getApplyFriendList(const std::string &uid,std::string &rsp);       //获取好友申请列表
    static bool getUserInfoById(const std::string &uid,std::string &rsp);       //获取好友申请列表
    static bool whetherAcceptAddFriend(const std::string &fromid,const std::string &toid,const std::string &remark,bool isAccept,std::string &rsp);       //是否同意添加好友
    static bool getFriendList(const std::string &uid,std::any &rsp);       //获取好友列表
    static bool sendMessage(const std::string &fromid,const std::string &toid,const std::string &text,const std::string &time,std::string &rsp);       //发送消息
    static bool getFriendMsg(const std::string &uid,const std::string &fid,int page,std::any &rsp);       //获取消息
    static bool getFriendgRemark(const std::string &uid,const std::string &fid,std::string &rsp);       //获取好友备注

private:
    static MysqlDao mysqlDao;
    MysqlDao(); 
    std::shared_ptr<MysqlConnectPool> pool_;
};
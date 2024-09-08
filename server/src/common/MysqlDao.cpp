#include "MysqlDao.h"
#include "global.h"
#include <regex>
#include <json/json.h>
#include <json/writer.h>
MysqlDao MysqlDao::mysqlDao;
bool MysqlDao::register_user(const std::string &account, const std::string &pwd, const std::string &nick, std::string &ret_str)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    
    try
    {
        auto stmt = std::unique_ptr<sql::PreparedStatement>{conn_ptr->prepareStatement("call reg_user(?,?,?,@res);")}; 
        stmt->setString(1,account);
        stmt->setString(2,pwd);
        stmt->setString(3,nick);
        stmt->execute();

        auto stmt_result = std::unique_ptr<sql::Statement>(conn_ptr->createStatement());
        std::unique_ptr<sql::ResultSet> result_set{stmt_result->executeQuery("select @res as result;")};
        if(result_set->next())
        {
            int n = result_set->getInt("result");
            switch (n)
            {
                case -1 : ret_str = "sql error"; return false;
                case 0  : ret_str = "用户已存在"; return false;
                default : return true;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        ret_str = "sql execption";
        return false;
    }
    return false;
}

bool MysqlDao::check_pwd(const std::string &account, const std::string &pwd, std::string &rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    std::regex r{R"(^\d+$)"};
    try
    {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("call check_pwd(?,?,@rsp)")); 
        stmt->setString(1,account);
        stmt->setString(2,pwd);
        stmt->execute();

                
        auto stmt_rsp = std::unique_ptr<sql::Statement>(conn_ptr->createStatement()); 
        std::unique_ptr<sql::ResultSet> result_set{ stmt_rsp->executeQuery("select @rsp as result") };
        if(result_set->next()){
            rsp = result_set->getString("result");
            return std::regex_match(rsp,r);
        }

        return false;
    }
    catch(const sql::SQLException& e)
    {
        std::cerr << e.what();        
    }
    return false; 
}

bool MysqlDao::save_user_head(const std::string &uid, const std::string &img, std::string &rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    try{
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("call uploadHeadImg(?,?,@rsp)"));
        stmt->setBigInt(1,uid);
        stmt->setString(2,img);
        stmt->execute();
        auto stmt_rsp = std::unique_ptr<sql::Statement>(conn_ptr->createStatement());
        auto result = std::unique_ptr<sql::ResultSet>(stmt_rsp->executeQuery("select @rsp as result"));
        if(result->next()){
            if(result->getString("result") == uid)
                return true;
        }
    }catch(const sql::SQLException &e){
        LOG_INFO("%s",e.what())
    }
    return false;
}

bool MysqlDao::getUserHead(const std::string &uid, std::string &rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    try{
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("call getUserHead(?,@rsp,@data)"));
        stmt->setString(1,uid);
        stmt->execute();
        auto stmt_rsp = std::unique_ptr<sql::Statement>(conn_ptr->createStatement());
        auto result = std::unique_ptr<sql::ResultSet>(stmt_rsp->executeQuery("select @rsp as id,@data as data;"));
        if(result->next()){
            if(result->getString("id") == "0"){
                rsp = "用户头像不存在";
                return false;
            }

            if(result->getString("id") == uid){
                rsp = result->getString("data");
                return true;
            }
        }
    }catch(const sql::SQLException &e){
        LOG_INFO("%s",e.what())
    }

    return false;
}

bool MysqlDao::getUserInfoByAccount(const std::string &account, std::string &rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    Json::Value dest;
    try
    {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("select `id` as id,`nick` as nick from user where `account` = ?;")); 
        stmt->setString(1,account);
        auto sql_data = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if(sql_data->next()){
            dest["id"] = sql_data->getUInt("id");
            dest["nick"] = std::string(sql_data->getString("nick"));
            rsp = json_writer.write(dest);
            return true;
        }
        rsp = "未查询到该用户";
        return false;
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }
    return false;
}

bool MysqlDao::requestAddFriend(const std::string &uid, const std::string &toid, const std::string &apply, const std::string &remark, std::string &rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    Json::Value dest;
    try
    {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("call requestAddFriend(?,?,?,?,@rsp)")); 
        stmt->setString(1,uid);
        stmt->setString(2,toid);
        stmt->setString(3,apply);
        stmt->setString(4,remark);
        stmt->execute();        

        auto stmt_query = std::unique_ptr<sql::Statement>(conn_ptr->createStatement()); 
        auto sql_rsp  = std::unique_ptr<sql::ResultSet>(stmt_query->executeQuery("select @rsp as result")); 
        if(sql_rsp->next()){
            rsp = sql_rsp->getString("result");
            if(rsp == uid)
                return true;
            return false;
        }
        return false;
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
        rsp = e.what();
    }
    return false;
}

bool MysqlDao::getApplyFriendList(const std::string & uid, std::string & rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    Json::Value dest;
    
    try
    {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("select fromid,applyInfo from friendapply where toid = ? and pending = true")); 
        stmt->setString(1,uid);
        auto sql_rsp = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        
        Json::Value json(Json::arrayValue);
        while (sql_rsp->next())
        {
            Json::Value item;
            item["fromid"] = sql_rsp->getString("fromid").asStdString();
            item["applyInfo"] = sql_rsp->getString("applyInfo").asStdString();
            json.append(item);
        }
        rsp = json_writer.write(json); 
        if(json.size() ==0)
            rsp = "没有新的好友申请";
        return json.size() != 0;
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }
    return false; 
}

bool MysqlDao::getUserInfoById(const std::string &uid, std::string &rsp)
{
    
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    Json::Value dest;
    try
    {

        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("select nick,account from user where id = ?")); 
        stmt->setString(1,uid);
        auto sql_rsp = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if(sql_rsp->next()){
            dest["account"] = sql_rsp->getString("account").asStdString();
            dest["nick"] = sql_rsp->getString("nick").asStdString();
            rsp = json_writer.write(dest);
            return true;
        }
        rsp = "未能查找到匹配数据";
        return false;

    }
    catch(const std::exception& e)
    {
        rsp = e.what();
        LOG_INFO("%s",e.what())
    }
    return false; 
}

bool MysqlDao::whetherAcceptAddFriend(const std::string &fromid, const std::string &toid, const std::string &remark, bool isAccept, std::string &rsp)
{

    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    try
    {

        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("call whetherAcceptAddFriend(?,?,?,?,@rsp)")); 
        stmt->setString(1,fromid);
        stmt->setString(2,toid);
        stmt->setString(3,remark);
        stmt->setBoolean(4,isAccept);
        stmt->execute();
        
        auto stmt_query = std::unique_ptr<sql::Statement>(conn_ptr->createStatement());
        auto result = std::unique_ptr<sql::ResultSet>(stmt_query->executeQuery("select @rsp as result"));
        if(result->next()){
            rsp = result->getString("result");
            return rsp == "ok";
        }
        return false;

    }
    catch(const std::exception& e)
    {
        rsp = e.what();
        LOG_INFO("%s",e.what())
    }
    return false;
}

bool MysqlDao::getFriendList(const std::string &uid, std::any &rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    Json::Value array(Json::arrayValue);
    try
    {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("select toid,remark from friends where fromid = ?;")); 
        stmt->setString(1,uid);
        auto result = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (result->next())
        {
            Json::Value item;
            item["id"] = result->getString("toid").asStdString(); 
            item["remark"] = result->getString("remark").asStdString(); 
            array.append(item);
        }

        if(array.size() ==0)
            rsp = "没有数据";
        else
            rsp = array;
        
        return array.size() != 0;
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
        rsp = e.what();
    }
    return false;
}

bool MysqlDao::sendMessage(const std::string &fromid, const std::string &toid, const std::string &text, const std::string &time, std::string &rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    try
    {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("call saveMsg(?,?,?,?,@rsp);")); 
        stmt->setString(1,fromid);
        stmt->setString(2,toid);
        stmt->setString(3,text);
        stmt->setString(4,time);
        stmt->execute();


        auto stmt_query = std::unique_ptr<sql::Statement>(conn_ptr->createStatement()); 
        auto result = std::unique_ptr<sql::ResultSet>(stmt_query->executeQuery("select @rsp as result;"));
        if(result->next()){
            rsp = result->getString("result"); 
            return rsp == "ok";
        }
        return false;
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }
    return false;
}

bool MysqlDao::getFriendMsg(const std::string &uid,const std::string &fid,int skipPage, std::any &rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    Json::Value array(Json::arrayValue);
    try
    {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement(R"(select * from (select * from(select fromid,toid,text,time from message where fromid = ? and toid = ? union select fromid,toid,text,time from message where fromid = ? and toid = ?)as t_1 order by time desc limit 20 offset ?) as t_2 order by time asc;)")); 
        stmt->setString(1,uid);
        stmt->setString(2,fid);
        stmt->setString(3,fid);
        stmt->setString(4,uid);
        stmt->setInt(5, skipPage*20);
        auto result = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (result->next())
        {
            Json::Value item;
            item["fromid"] = result->getString("fromid").asStdString();
            item["toid"] = result->getString("toid").asStdString();
            item["text"] = result->getString("text").asStdString();
            item["time"] = result->getString("time").asStdString();
            array.append(item); 
        }

        if(array.size() > 0)
            rsp = array;
        else
            rsp = "没有消息";
        
        return array.size() != 0;
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
        rsp = e.what();
    }
    return false;
}

bool MysqlDao::getFriendgRemark(const std::string &uid, const std::string &fid, std::string &rsp)
{
    auto unique_ptr = mysqlDao.pool_->getConn(); //防止连接被返回
    auto &conn_ptr = *(unique_ptr.get());
    try
    {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn_ptr->prepareStatement("select remark from friends where fromid = ? and toid =?")); 
        stmt->setString(1,uid);
        stmt->setString(2,fid);
        auto result = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if(result->next()){
            rsp = result->getString("remark");
            return true;
        }
        return false;
    }
    catch(const std::exception& e)
    {
        LOG_INFO("%s",e.what())
    }

    return false; 
}

MysqlDao::MysqlDao():pool_(std::make_shared<MysqlConnectPool>())
{

}

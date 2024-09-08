#pragma once
#include <iostream>
#include <thread>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <queue>
#include <cppconn/prepared_statement.h>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <functional>
#include <chrono>
#include "noncopyable.h"


class MysqlConnectPool;
class MysqlConn:public noncopyable{
    friend class MysqlConnectPool;
public:
    ~MysqlConn(){
        if(conn_){
            conn_->close();
            delete conn_;
            conn_ = nullptr;
        }
    }
    // sql::Connection& operator*(){  return *conn_; }
    sql::Connection* operator->(){
        return conn_;
    }
    sql::Connection& operator*(){
        return *conn_;
    }
    void refreshLastTime(){ last_time_ = std::chrono::system_clock::now(); };
private:
    explicit MysqlConn(sql::Connection *conn):conn_(conn),last_time_(std::chrono::system_clock::now()){};
    sql::Connection *conn_;
    std::chrono::time_point<std::chrono::system_clock> last_time_;
    std::uint32_t idle_time_;
};
using MysqlConnPtr = std::unique_ptr<MysqlConn,std::function<void(MysqlConn*)>>;



class MysqlConnectPool: public noncopyable, public std::enable_shared_from_this<MysqlConnectPool>{
public:
    MysqlConnPtr getConn();
    MysqlConnectPool(uint16_t size = std::thread::hardware_concurrency());
    ~MysqlConnectPool();
private:
    int size_;
    std::queue<MysqlConn*> connects_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::string host_;
    std::string port_;
    std::string user_;
    std::string pwd_;
    std::string db_;
    std::string idle_time_;
};

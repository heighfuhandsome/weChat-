#include "MysqlConnectPool.h"
#include "Config.h"

MysqlConnectPool::MysqlConnectPool(uint16_t size):size_(size)
{
    host_ = Configuration::get("Mysql.host");
    port_ = Configuration::get("Mysql.port");
    auto url = "tcp://" + host_ + ":" + port_;
    user_ = Configuration::get("Mysql.user");
    pwd_ = Configuration::get("Mysql.pwd");
    db_ = Configuration::get("Mysql.db");
    idle_time_ = Configuration::get("Mysql.idleTime");
    auto driver = sql::mysql::get_driver_instance();
    for(int i=0;i<size_;i++)
    {
        auto conn = driver->connect(url,user_,pwd_);
        conn->setSchema(db_);
        if(conn == nullptr)
            throw std::exception();
        auto myConn = new MysqlConn{conn};
        myConn->idle_time_ = std::stoul(idle_time_);
        connects_.push(myConn);
    }
}

MysqlConnectPool::~MysqlConnectPool()
{
    std::lock_guard<std::mutex> guard{ mutex_};
    while (!connects_.empty())
    {
        auto ptr = connects_.front();
        connects_.pop();
        delete ptr;
    }
}

MysqlConnPtr MysqlConnectPool::getConn()
{
    std::unique_lock<std::mutex> lock{ mutex_};
    cond_.wait(lock,[=]{
        return !connects_.empty();
    });

    auto connPtr = connects_.front();
    connects_.pop();
    auto dif_sends = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - connPtr->last_time_);
    if(dif_sends.count() >= connPtr->idle_time_){
        //如果空闲时间超过 Mysql.idleTime , 重新建立此链接
        delete connPtr;
        auto conn = sql::mysql::get_driver_instance()->connect("tcp://"+host_+":"+port_,user_,pwd_);
        conn->setSchema(db_);
        connPtr = new MysqlConn{conn};
        connPtr->idle_time_ = std::stoul(idle_time_);
    }
    return MysqlConnPtr{connPtr, [self=shared_from_this()](MysqlConn *conn){
        std::lock_guard<std::mutex> guard{ self->mutex_};
        conn->refreshLastTime();
        self->connects_.push(conn);
        self->cond_.notify_one();
    }};
}
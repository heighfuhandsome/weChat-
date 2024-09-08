#include "RedisConnPool.h"
#include "Logger.h"
#include "Config.h"
std::unique_ptr<RedisConnPoll> RedisConnPoll::pool = nullptr;


RedisConn RedisConnPoll::getConn()
{
    static std::once_flag flag;
    std::call_once(flag,[&]{
        pool = std::unique_ptr<RedisConnPoll>(new RedisConnPoll);
    });

    redisContext *conn;
    {
        std::unique_lock<std::mutex> lock{ pool->mutex_ };
        pool->cond_.wait(lock,[&]{
            return !pool->conns_.empty();
        });
        conn = pool->conns_.front();
        pool->conns_.pop();
    }

    return RedisConn(conn,[&](redisContext *ctx){
        std::lock_guard<std::mutex> guard{ pool->mutex_ };
        pool->conns_.push(ctx);
        pool->cond_.notify_one();
    });

}

RedisConnPoll::RedisConnPoll(int size)
{
    auto host = Configuration::get("Redis.host");
    auto port = Configuration::get("Redis.port");
    for(int i=0;i<size;i++){
        auto conn = redisConnect(host.c_str(),std::stoi(port));
        if(!conn){
            LOG_INFO("redis conn fail")
            exit(0);
        }
        conns_.push(conn);
    }
}
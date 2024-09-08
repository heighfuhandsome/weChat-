#pragma once
#include "noncopyable.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <hiredis/hiredis.h>
#include <memory>
#include <condition_variable>
using RedisConn = std::unique_ptr<redisContext,std::function<void(redisContext*)>>;

class RedisConnPoll : public noncopyable{
public:
    static RedisConn getConn();
protected:
    RedisConnPoll(int size = std::thread::hardware_concurrency());
    
private:
    static std::unique_ptr<RedisConnPoll> pool;
    
    std::condition_variable cond_;
    std::queue<redisContext*> conns_;
    std::mutex mutex_;
};
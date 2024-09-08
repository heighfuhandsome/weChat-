#pragma once
#include "noncopyable.h"
#include "global.h"
#include <mutex>
#include <vector>
#include <memory>
#include <thread>
#include <iostream>

class IoContextPool : public noncopyable{
    using ThreadPtr = std::unique_ptr<std::thread>;
public:
    static net::io_context& GetInstance();
private:
    IoContextPool(uint8_t pool_size = std::thread::hardware_concurrency() -1 );
    // IoContextPool(uint8_t pool_size = 1);
    
    std::vector<ThreadPtr> threads_;
    std::vector<net::io_context::work> wroks_;
    std::vector<net::io_context> io_context_pool_;
    std::mutex mutex_;
    uint8_t size_;
    uint8_t index_;
};

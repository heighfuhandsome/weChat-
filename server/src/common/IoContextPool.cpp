#include "IoContextPool.h"

net::io_context &IoContextPool::GetInstance()
{
    static IoContextPool pool;
    // std::lock_guard guard(pool.mutex_);
    auto &ioc = pool.io_context_pool_[pool.index_++];
    pool.index_ %= pool.size_;
    return ioc;
}

IoContextPool::IoContextPool(uint8_t pool_size) : io_context_pool_(pool_size), size_(pool_size), index_(0)
{
    for(int i=0;i<size_;i++)
    {
        wroks_.emplace_back( net::io_context::work{ io_context_pool_[i] });
        auto thread = new std::thread{[=]{
            io_context_pool_[i].run();
            std::cout << std::this_thread::get_id()  << " thread exit\n";
        }};
        threads_.push_back(std::unique_ptr<std::thread>{thread});
    }
}
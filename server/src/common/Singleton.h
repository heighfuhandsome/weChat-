#pragma once
#include "noncopyable.h"
#include <memory>
#include <mutex>
template<class T>
class Singleton{
public:
    Singleton<T>(const Singleton<T> &) = delete;
    Singleton<T>& operator=(const Singleton<T> &) = delete;
    static T& GetInstance();

protected:
    Singleton<T>() = default;
    static std::unique_ptr<T> instance_;
};


template <class T>
std::unique_ptr<T> Singleton<T>::instance_ = nullptr;

template <class T>
inline T &Singleton<T>::GetInstance()
{
    static std::once_flag flag;
    std::call_once(flag,[&]{
        instance_ = std::unique_ptr<T>(new T);
    });

    return *instance_;
}

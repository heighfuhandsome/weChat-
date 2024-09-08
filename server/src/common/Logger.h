#pragma once
#include <stdio.h>
#include <cstring>
#include <cstdarg>
#include <mutex>
#include <string>

enum class LoggerLeveL{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATANL
};

class Logger
{
public:
    Logger(const Logger &) = delete;
    void operator=(const Logger &) = delete;
    static Logger &getLogger()
    {
        thread_local Logger Logger;
        return Logger;
    }

    Logger& setLevel(LoggerLeveL level)
    {
        level_ = level;
        return *this;
    }

    void log(const char *fmt,const char *file ,int line,...)
    {
        char buff[256]{0};
        size_t len;
        sprintf(buff, "%s ", getStrByLevel(level_));
        len = strlen(buff);
        sprintf(buff + len, "%s %d ",file, line);
        len = strlen(buff);
        getTimeStr(buff + len, 256 - len);

        va_list list;
        va_start(list,line);
        len = strlen(buff);
        vsnprintf(buff + len, 256 - len, fmt, list);
        va_end(list);
        {
            std::lock_guard<std::mutex> guard(mutex_);
            if (level_ == LoggerLeveL::DEBUG)
            {
#ifdef LOG_DEBUG
                printf("%s\n", buff);
#endif
            }
            else
            {
                printf("%s\n", buff);
            }
        }

        if (level_ == LoggerLeveL::FATANL)
        {
            exit(-1);
        }
    }
    ~Logger() = default;

private:
    Logger() = default;
    size_t getTimeStr(char *buff, size_t size)
    {
        time_t t;
        time(&t);
        struct tm *tm_t = gmtime(&t);
        tm_t->tm_hour += 8;
        if (tm_t->tm_hour > 23)
        {
            tm_t->tm_mday += 1;
            tm_t->tm_hour -= 24;
        }
        return strftime(buff, size, "%Y-%m-%d %H:%M:%S : ", tm_t);
    }

    const char *getStrByLevel(LoggerLeveL level)
    {
        switch (level)
        {
        case LoggerLeveL::DEBUG:
            return "[DEBUG]";
        case  LoggerLeveL::INFO:
            return "[INFO]";
        case  LoggerLeveL::WARN:
            return "[WARN]";
        case  LoggerLeveL::ERROR:
            return "[ERROR]";
        case  LoggerLeveL::FATANL:
            return "[FATAL]";
        default:
            return "[NONE]";
        }
    }

private:
    LoggerLeveL level_;
    std::mutex mutex_;
};



#define LOG_DEBUG(fmt,...) Logger::getLogger().setLevel(LoggerLeveL::DEBUG).log(fmt,__FILE__,__LINE__,##__VA_ARGS__);
#define LOG_INFO(fmt,...) Logger::getLogger().setLevel(LoggerLeveL::INFO).log(fmt,__FILE__,__LINE__,##__VA_ARGS__);
#define LOG_WARN(fmt,...) Logger::getLogger().setLevel(LoggerLeveL::WARN).log(fmt,__FILE__,__LINE__,##__VA_ARGS__);
#define LOG_ERROR(fmt,...) Logger::getLogger().setLevel(LoggerLeveL::ERROR).log(fmt,__FILE__,__LINE__,##__VA_ARGS__);
#define LOG_FATAL(fmt,...) Logger::getLogger().setLevel(LoggerLeveL::FATANL).log(fmt,__FILE__,__LINE__,##__VA_ARGS__);
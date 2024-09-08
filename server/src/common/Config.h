#pragma once
#include "noncopyable.h"
#include <string>
#include <mutex>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <memory>
class Configuration: public noncopyable{
public:
    static std::string get(const std::string key);

private:
    static std::unique_ptr<Configuration> instance_;
    Configuration();
    std::mutex mutex_;
    boost::property_tree::ptree config_;
};
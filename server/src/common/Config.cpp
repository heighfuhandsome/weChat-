#include "Config.h"
#include <iostream>

std::unique_ptr<Configuration> Configuration::instance_ = nullptr;

std::string Configuration::get(const std::string key)
{

    static std::once_flag flags;
    std::call_once(flags,[&]{
        instance_ = std::unique_ptr<Configuration>(new Configuration);
    });

    std::lock_guard<std::mutex> guard{instance_->mutex_ };
    auto option = instance_->config_.get_child_optional(key);
    if(option){
        return option->data();
    }else{
        return "";
    }
}

Configuration::Configuration()
{
    try
    {
        boost::property_tree::ini_parser::read_ini("../config.ini",config_);
    }
    catch(const std::exception& e)
    {
        std::cout << e.what();
    }
    
}

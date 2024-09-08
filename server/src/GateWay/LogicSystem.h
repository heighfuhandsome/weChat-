#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>
#include <unordered_map>
#include "noncopyable.h"

class HttpSession;
using HttpGetHandler = std::function<bool(std::shared_ptr<HttpSession>,std::unordered_map<std::string,std::string>)>;
using HttpPostHandler = std::function<bool(std::shared_ptr<HttpSession>,const std::string &body)>;


bool urlDecode(const std::string& url,std::string &dest);
unsigned char FromHex(unsigned char x);

class LogicSystem:noncopyable {
public:
    static LogicSystem& getInstance();
    bool handleRequest(std::shared_ptr<HttpSession> session);
    ~LogicSystem() = default;
private:
    LogicSystem();
    std::unordered_map<std::string,HttpGetHandler> getHandlerMap_;
    std::unordered_map<std::string,HttpPostHandler> postHandlerMap_;
};

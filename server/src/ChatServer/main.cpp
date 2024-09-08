#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Logger.h"
#include "TcpServer.h"
#include "Config.h"
int main(int argc,char **argv){
    if(argc < 2){
        std::cout << "missing params\n";
    }

    std::string port = Configuration::get(std::string{argv[1]} + ".port");
    if(port == ""){
        printf("error params");
        return 0;
    }else{
        LOG_INFO("%s", argv[1])
    }
    TcpServer s(std::stoul(port));
    s.SetName(argv[1]);
    LOG_INFO("%s : %s","chatServer is startted , linsten port", port.c_str())
    s.run();

    return 0;
}
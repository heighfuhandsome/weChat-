#include "StatusServer.h"

void runServer(){
    boost::property_tree::ptree config;
    boost::property_tree::ini_parser::read_ini("../config.ini",config);
    std::string addr = config.get<std::string>("StatusServer.host") + ":" + config.get<std::string>("StatusServer.port");
    grpc::ServerBuilder builder;
    StatusServerImpl impl;
    builder.AddListeningPort(addr,grpc::InsecureServerCredentials());
    builder.RegisterService(&impl);
    auto server = builder.BuildAndStart();
    server->Wait();
}

int main(){
    runServer();
    return 0;
}
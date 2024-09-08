#include "StatusServerClient.h"
#include "Logger.h"
std::unique_ptr<StatusServerClient> StatusServerClient::client = nullptr;

bool StatusServerClient::GetChatServer(std::string uid, Json::Value &rsp)
{
    std::unique_ptr<Server::StatusServer::Stub> stub = StatusServerClient::GetStube();
    Server::UserInfo user_info;
    Server::ChatServerInfo chat_server_info;
    ::grpc::ClientContext context;
    user_info.set_uid(uid);
    auto stat = stub->GetChartServer(&context,user_info,&chat_server_info);
    StatusServerClient::RetuenStube(std::move(stub));
    if(stat.ok()){
        rsp["host"] = chat_server_info.host();
        rsp["port"] = chat_server_info.port();
        rsp["token"] = chat_server_info.token();
        return true;
    }
    return false;
}

bool StatusServerClient::Login(std::string uid, std::string token,std::string host,Json::Value &rsp)
{
    grpc::ClientContext ctx;
    Server::LoginReq req;
    Server::LoginRsp grpc_rsp;
    req.mutable_user()->set_uid(uid);
    req.set_token(token);
    req.set_host(host);

    auto stub = StatusServerClient::GetStube();
    auto stat = stub->Login(&ctx,req,&grpc_rsp);
    StatusServerClient::RetuenStube(std::move(stub));
    if(!stat.ok())
        return false;
    
    rsp["uid"] = grpc_rsp.user().uid();
    rsp["token"] = grpc_rsp.token();
    return true;
}


std::unique_ptr<Server::StatusServer::Stub> StatusServerClient::GetStube()
{
    initClient();
    std::unique_ptr<Server::StatusServer::Stub> stub = nullptr;
    {
        std::lock_guard<std::mutex> { client->mutex_ };
        stub = std::move(client->stubs_.front());
        client->stubs_.pop();
    }
    return stub;
}

void StatusServerClient::RetuenStube(std::unique_ptr<Server::StatusServer::Stub> stub)
{
    std::lock_guard<std::mutex> { client->mutex_ };
    client->stubs_.push(std::move(stub));
}

void StatusServerClient::initClient()
{
    static std::once_flag flag;
    std::call_once(flag,[&]{
        client = std::unique_ptr<StatusServerClient>(new StatusServerClient);
    });
}

StatusServerClient::StatusServerClient(uint8_t size_)
{
    boost::property_tree::ptree p;
    try
    {
        boost::property_tree::ini_parser::read_ini("../config.ini",p);
        auto host = p.get<std::string>("StatusServer.host");
        auto port = p.get<std::string>("StatusServer.port");
        auto channel = grpc::CreateChannel(host+":"+port,grpc::InsecureChannelCredentials());
        for(int i=0;i<size_;i++)
            stubs_.push(Server::StatusServer::NewStub(channel));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

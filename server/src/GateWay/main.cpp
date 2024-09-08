#include <iostream>
#include "HttpServer.h"
#include "Config.h"

int main(){
    net::io_context ioc;
    HttpServer s(ioc,80);
    ioc.run();
    return 0;
}
#pragma once

#include <string>

namespace cqrs {

    struct AppConfig
    {
        std::string publisherAddr;
        std::string cmdGatewayAddr;
        std::string cmdPublisherAddr;
        std::string evtGatewayAddr;
        std::string evtPublisherAddr;
    };

}
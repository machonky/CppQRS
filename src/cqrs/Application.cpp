#include <iostream>
#include <string>
#include <utility>
#include <thread>
#include <caf/all.hpp>
#include <zmq.hpp>
#include "cqrs/Application.h"
#include "cqrs/AppConfig.h"
#include "cqrs/AppRootActor.h"
#include "cqrs/msg/QueryConfig.h"

namespace cqrs {
    
    int Application::run(int argc, const char* argv[])
    {
        zmq::context_t networkContext;

        caf::actor_system_config config;
        caf::actor_system system{ config };

        AppConfig appConfig{ "inproc://CppQRS" };
        caf::actor appRoot = system.spawn<AppRootActor>(std::ref(networkContext), std::cref(appConfig));

        // Send some messages to the publishing actor
        caf::anon_send(appRoot, std::string("Hello CppQRS"));
        caf::anon_send(appRoot, msg::QueryConfig());

        return EXIT_SUCCESS;
    }
}

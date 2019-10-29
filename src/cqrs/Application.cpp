
#include <string>
#include <utility>
#include <caf/all.hpp>
#include "cqrs/Application.h"
#include "cqrs/AppConfig.h"
#include "cqrs/AppRootActor.h"
#include "cqrs/ZmqReactor.h"

namespace cqrs {

    Application::Application()
    {}

    int Application::run(int argc, const char* argv[])
    {
        zmq::context_t networkContext;

        caf::actor_system_config config;
        caf::actor_system system{ config };

        AppConfig appConfig{ "inproc://CppQRS" };
        caf::actor appRoot = system.spawn<AppRootActor>(std::ref(networkContext), std::cref(appConfig));

        // Send some messages to the publishing actor
        caf::anon_send(appRoot, std::string("Hello CppQRS"));

        std::cin.get();

        return EXIT_SUCCESS;
    }
}

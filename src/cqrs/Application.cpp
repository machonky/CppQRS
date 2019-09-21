#include <iostream>
#include <string>
#include <type_traits>
#include <caf/all.hpp>
#include "cqrs/Application.h"
#include "cqrs/AppRootActor.h"

namespace cqrs {

    int Application::run(int argc, const char* argv[])
    {
        caf::actor_system_config config;
        caf::actor_system system{ config };

        caf::actor appRoot = system.spawn<AppRootActor>();
        caf::anon_send(appRoot, std::string("Hello CppQRS"));

        return EXIT_SUCCESS;
    }
}

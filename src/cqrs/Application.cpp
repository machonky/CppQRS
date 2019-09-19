#include <iostream>
#include "cqrs/Application.h"
#include "logger/Logger.h"

namespace cqrs {

    Application::Application(const logger::Logger& logger)
        : logger(logger)
    {}

    int Application::run(int argc, const char* argv[])
    {
        logger.detail("Application start");
        std::cout << "Hello CQRS" << std::endl;
        return EXIT_SUCCESS;
    }
}

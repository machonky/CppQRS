#include <iostream>
#include "Application.h"

namespace cqrs {

    int Application::run(int argc, const char* argv[])
    {
        std::cout << "Hello CQRS" << std::endl;
        return EXIT_SUCCESS;
    }
}

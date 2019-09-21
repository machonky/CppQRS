#include <iostream>
#include "cqrs/Application.h"

int main(int argc, const char* argv[]) 
{
    cqrs::Application theApp;
    theApp.run(argc, argv);

    std::cin.get();

    return EXIT_SUCCESS;
}
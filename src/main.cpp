#include <iostream>
#include "cqrs/Application.h"
#include "logger/ConsoleLogger.h"

int main(int argc, const char* argv[]) 
{
    logger::ConsoleLogger theLog;
    cqrs::Application theApp(theLog);
    theApp.run(argc, argv);

    std::cin.get();

    return 0;
}
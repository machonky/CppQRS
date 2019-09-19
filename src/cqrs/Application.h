#pragma once

namespace logger {
    class Logger;
}

namespace cqrs {

    class Application 
    {
        const logger::Logger& logger;

    public:
        Application(const logger::Logger& logger);
        int run(int argc, const char* argv[]);
    };
}


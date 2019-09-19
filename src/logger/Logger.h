#pragma once

namespace logger {

    class Logger
    {
    public:
        virtual void trace(const char* msg) const = 0;
        virtual void detail(const char* msg) const = 0;
        virtual void warn(const char* msg) const = 0;
        virtual void error(const char* msg) const = 0;
        virtual void alert(const char* msg) const = 0;
    };

}

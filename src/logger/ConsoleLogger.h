#pragma once

#include "Logger.h"

namespace logger {

    class ConsoleLogger : public Logger
    {
    public:
        void trace(const char* msg) const override;
        void detail(const char* msg) const override;
        void warn(const char* msg) const override;
        void error(const char* msg) const override;
        void alert(const char* msg) const override;
    };

}


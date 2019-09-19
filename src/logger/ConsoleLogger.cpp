#include <iostream>
#include "ConsoleLogger.h"

namespace logger {

    void writeLog(const char* level, const char* msg)
    {
        std::cerr << "[" << level << "] " << msg << std::endl;
    }

    void ConsoleLogger::trace(const char* msg) const
    {
        writeLog("TRACE ", msg);
    }

    void ConsoleLogger::detail(const char* msg) const
    {
        writeLog("DETAIL", msg);
    }

    void ConsoleLogger::warn(const char* msg) const
    {
        writeLog("WARN  ", msg);
    }

    void ConsoleLogger::error(const char* msg) const
    {
        writeLog("ERROR ", msg);
    }

    void ConsoleLogger::alert(const char* msg) const
    {
        writeLog("ALERT ", msg);
    }
}

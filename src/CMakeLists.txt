cmake_minimum_required (VERSION 3.13.0)

include_directories("../src")

add_executable(CppQRS main.cpp cqrs/Application.cpp logger/Logger.h logger/ConsoleLogger.h logger/ConsoleLogger.cpp)

cmake_minimum_required (VERSION 3.13.0)

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories($ENV("INCLUDE"))
include_directories("../src")

add_executable(CppQRS 
	main.cpp 
	cqrs/Application.cpp 
	cqrs/AppRootActor.cpp 
	cqrs/AppRootActor.h 
	logger/Logger.h 
	logger/ConsoleLogger.h 
	logger/ConsoleLogger.cpp
)

link_directories($ENV("LIB"))
target_link_libraries(CppQRS IPHLPAPI caf_core_static libzmq-v141-mt-s-4_3_2)

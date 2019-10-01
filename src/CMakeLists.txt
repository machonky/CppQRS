cmake_minimum_required (VERSION 3.13.0)

set(CMAKE_VERBOSE_MAKEFILE ON)

#ZeroMq, CAF & boost are in the "INCLUDE" env variable
include_directories($ENV("INCLUDE"))
include_directories("../src")

add_executable(CppQRS 
	main.cpp 
	cqrs/Application.cpp 
	cqrs/AppRootActor.cpp 
	cqrs/AppRootActor.h 
	cqrs/ZmqPublisherActor.h
	cqrs/ZmqPublisherActor.cpp
	cqrs/ZmqTextMessageSink.cpp
	cqrs/ZmqTextMessageSink.h
	cqrs/ZmqSubscriberMessageSource.cpp
	cqrs/ZmqSubscriberMessageSource.h
	cqrs/msg/ZmqMessagePtr.h
	cqrs/msg/QueryConfig.h
	#logger/Logger.h 
	#logger/ConsoleLogger.h 
	#logger/ConsoleLogger.cpp
)

#ZeroMq, CAF & boost libs are in the "LIB" env variable
link_directories($ENV("LIB"))
target_link_libraries(CppQRS IPHLPAPI caf_core_static libzmq-v141-mt-4_3_2)

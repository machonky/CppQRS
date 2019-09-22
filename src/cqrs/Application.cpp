#include <iostream>
#include <string>
#include <utility>
#include <thread>
#include <caf/all.hpp>
#include <zmq.hpp>
#include "cqrs/Application.h"
#include "cqrs/AppConfig.h"
#include "cqrs/AppRootActor.h"
#include "cqrs/msg/QueryConfig.h"

namespace cqrs {
    
    int Application::run(int argc, const char* argv[])
    {
        zmq::context_t networkContext;

        caf::actor_system_config config;
        caf::actor_system system{ config };

        AppConfig appConfig{ "inproc://CppQRS" };
        caf::actor appRoot = system.spawn<AppRootActor>(std::ref(networkContext), std::cref(appConfig));

        //*/
        // Clumsy non-scalable subscriber hack follows to test our publisher
        zmq::socket_t subscriberSocket(networkContext, zmq::socket_type::sub);
        subscriberSocket.connect(appConfig.publisherAddr);
        subscriberSocket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

        std::vector<zmq::pollitem_t> pollItems = { {subscriberSocket, 0, ZMQ_POLLIN, 0} };
        std::thread subscriberThread([&]()
            {
                while (true)
                {
                    zmq::message_t rxMsg;
                    zmq::poll(pollItems.data(), 1, -1); // poll blocks -1 infinite timeout
                    if (pollItems[0].revents & ZMQ_POLLIN) 
                    {
                        // received something on the first (only) socket
                        subscriberSocket.recv(rxMsg);
                        std::string receivedMessage;
                        receivedMessage.assign(rxMsg.data<char>(), rxMsg.size());
                        std::cout << "Subscriber Received: " << receivedMessage << std::endl;
                        break;
                    }
                }
            }
        );
        //*/
        // Send some messages to the publishing actor
        caf::anon_send(appRoot, std::string("Hello CppQRS"));
        caf::anon_send(appRoot, msg::QueryConfig());

        subscriberThread.join();

        return EXIT_SUCCESS;
    }
}

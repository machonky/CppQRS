#pragma once

#include <zmq.hpp>

namespace cqrs {

    struct Application 
    {
        Application();
        int run(int argc, const char* argv[]);

        void onCommand(zmq::socket_ref s) {}
        void onEvent(zmq::socket_ref s) {}

    private:

        zmq::socket_t cmdGateway;
        zmq::socket_t cmdPublisher;
        zmq::socket_t evtPublisher;
    };
}


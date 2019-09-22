#pragma once

#include <string>
#include <caf/all.hpp>
#include <zmq.hpp>

namespace cqrs {

    class ZmqPublisherActor : public caf::event_based_actor
    {
        zmq::socket_t publisherSocket;
    public:
        ZmqPublisherActor(caf::actor_config& actorConfig, zmq::context_t& networkContext, const std::string& socketAddr);

    protected:
        caf::behavior make_behavior() override;
    };

}

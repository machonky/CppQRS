#include "cqrs/ZmqPublisherActor.h"
#include "cqrs/msg/ZmqMessagePtr.h"

namespace cqrs {

    ZmqPublisherActor::ZmqPublisherActor(caf::actor_config& actorConfig, zmq::context_t& networkContext, const std::string& socketAddr)
        : caf::event_based_actor(actorConfig)
        , publisherSocket(networkContext, zmq::socket_type::pub)
    {
        publisherSocket.bind(socketAddr);
        caf::aout(this) << "Binding publisher to: \""<< socketAddr <<"\""<<std::endl;
    }

    caf::behavior ZmqPublisherActor::make_behavior()
    {
        return caf::message_handler
        {
            [&](ZmqMessagePtr msg)
            {
                publisherSocket.send(*msg);
            },
            [&](ZmqMultipartPtr msg)
            {
                msg->send(publisherSocket);
            }
        };
    }

}
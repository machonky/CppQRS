#include <string>
#include <iostream>
#include "cqrs/AppRootActor.h"
#include "cqrs/AppConfig.h"
#include "cqrs/ZmqPublisherActor.h"
#include "cqrs/msg/QueryConfig.h"
#include "cqrs/msg/ZmqMessagePtr.h"

namespace cqrs {

    AppRootActor::AppRootActor(caf::actor_config& actorConfig, zmq::context_t& networkContext, const AppConfig& appConfig)
        : caf::event_based_actor(actorConfig)
        , appConfig(appConfig)
    {
        publisher = spawn<ZmqPublisherActor>(std::ref(networkContext), std::cref(appConfig.publisherAddr));
    }

    caf::behavior AppRootActor::make_behavior()
    {
        return caf::message_handler 
        {
            [&](const std::string& value) 
            {
                caf::aout(this) << "Publishing \""<< value <<"\""<< std::endl;
                ZmqMessagePtr msg = makeZmqMessagePtr(value.data(), value.size());
                send(publisher, msg);
            },
            [&](msg::QueryConfig) 
            { 
                caf::aout(this) << "Publisher address: " << appConfig.publisherAddr << std::endl;
            }
        };
    }

}

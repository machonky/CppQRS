#include <string>
#include <iostream>
#include "cqrs/AppRootActor.h"
#include "cqrs/AppConfig.h"
#include "cqrs/ZmqPublisherActor.h"
#include "cqrs/ZmqSubscriberActor.h"
#include "cqrs/msg/QueryConfig.h"
#include "cqrs/msg/ZmqMessagePtr.h"

namespace cqrs {

    AppRootActor::AppRootActor(caf::actor_config& actorConfig, zmq::context_t& networkContext, const AppConfig& appConfig)
        : caf::event_based_actor(actorConfig)
        , appConfig(appConfig)
    {
        std::string subscriptionTopic{ "" };
        publisher = spawn<ZmqPublisherActor , caf::linked>(std::ref(networkContext), std::cref(appConfig.publisherAddr));
        subscriber = spawn<ZmqSubscriberActor, caf::linked>(std::ref(networkContext), std::cref(appConfig.publisherAddr), std::cref(subscriptionTopic));

        std::string multiTopic{ "com.example" }; // this is a broader topic than com.example.specific so it will respond too
        multiSubscriber = spawn<ZmqSubscriberActor, caf::linked>(std::ref(networkContext), std::cref(appConfig.publisherAddr), std::cref(multiTopic));

        std::string newTopic = "com.example.specific";
        special = spawn<ZmqSubscriberActor, caf::linked>(std::ref(networkContext), std::cref(appConfig.publisherAddr), std::cref(newTopic));
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

                ZmqMultipartPtr multi = makeZmqMultipartPtr();
                multi->pushstr("com.example.specific");
                multi->addstr(value);
                send(publisher, multi);
            },
            [&](msg::QueryConfig) 
            { 
                caf::aout(this) << "Publisher address: " << appConfig.publisherAddr << std::endl;
            }
        };
    }

}

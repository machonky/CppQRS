#include <string>
#include <iostream>
#include "cqrs/AppRootActor.h"
#include "cqrs/AppConfig.h"
#include "cqrs/ZmqPublisherActor.h"
#include "cqrs/ZmqSubscriberMessageSource.h"
#include "cqrs/ZmqTextMessageSink.h"
#include "cqrs/msg/QueryConfig.h"
#include "cqrs/msg/ZmqMessagePtr.h"
#include "cqrs/msg/Stream.h"

namespace cqrs {

    AppRootActor::AppRootActor(caf::actor_config& actorConfig, zmq::context_t& networkContext, const AppConfig& appConfig)
        : caf::event_based_actor(actorConfig)
        , appConfig(appConfig)
    {
        publisher = spawn<ZmqPublisherActor , caf::linked>(std::ref(networkContext), std::cref(appConfig.publisherAddr));

        std::string specificTopic{ "com.example.specific" };
        caf::actor specificSource = 
            spawn<ZmqSubscriberMessageSource, caf::linked>(std::ref(networkContext), std::cref(appConfig.publisherAddr), std::cref(specificTopic));
        caf::actor specificSink = spawn<ZmqTextMessageSink, caf::linked>();

        // Assemble the pipeline
        multiSubscriber = specificSink * specificSource;

        caf::anon_send(multiSubscriber, msg::StartStream());
        caf::aout(this) << "Sent stream handshake to com.example.specific pipeline" << std::endl;
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

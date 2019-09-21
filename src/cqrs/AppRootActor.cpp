#include <string>
#include <iostream>
#include "AppRootActor.h"

namespace cqrs {

    AppRootActor::AppRootActor(caf::actor_config& actorConfig)
        : caf::event_based_actor(actorConfig)
    {}

    caf::behavior AppRootActor::make_behavior()
    {
        return caf::message_handler {
            [&](const std::string& value) {
                caf::aout(this) << value << std::endl;
            }
        };
    }
}

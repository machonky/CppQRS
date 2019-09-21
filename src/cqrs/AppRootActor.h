#pragma once

#include <caf/all.hpp>

namespace cqrs {

    class AppRootActor : public caf::event_based_actor
    {
    public:
        AppRootActor(caf::actor_config& actorConfig);
    protected:
        caf::behavior make_behavior() override;
    };

}
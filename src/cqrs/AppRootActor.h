#pragma once

#include <caf/all.hpp>
#include <zmq.hpp>
#include "AppConfig.h"

namespace cqrs {

    struct AppConfig;

    class AppRootActor : public caf::event_based_actor
    {
        const AppConfig appConfig;
        caf::actor publisher;

    public:
        AppRootActor(caf::actor_config& actorConfig, zmq::context_t& networkContext, const AppConfig& appConfig);
    protected:
        caf::behavior make_behavior() override;
    };

}
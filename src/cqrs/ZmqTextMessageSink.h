#pragma once

#include <caf/all.hpp>
#include "cqrs/msg/ZmqMessagePtr.h"

namespace cqrs {

    class ZmqTextMessageSink : public caf::event_based_actor
    {
    public:
        ZmqTextMessageSink(caf::actor_config& actorConfig);

    protected:
        caf::behavior make_behavior() override;

    private:
        caf::make_sink_result<ZmqMessagePtr> makeZmqMessageSink(ZmqMessagePtrStream in);
        caf::make_sink_result<ZmqMultipartPtr> makeZmqMultipartSink(ZmqMultipartPtrStream in);
    };

}
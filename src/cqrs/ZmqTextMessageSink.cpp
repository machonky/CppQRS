#include "cqrs/ZmqTextMessageSink.h"

namespace cqrs {

    ZmqTextMessageSink::ZmqTextMessageSink(caf::actor_config& actorConfig)
        : caf::event_based_actor(actorConfig)
    {}

    caf::behavior ZmqTextMessageSink::make_behavior()
    {
        return {
            [&](ZmqMessagePtrStream in) {
                return makeZmqMessageSink(in);
            },
            [&](ZmqMultipartPtrStream in) {
                return makeZmqMultipartSink(in);
            }
        };
    }

    caf::make_sink_result<ZmqMessagePtr> ZmqTextMessageSink::makeZmqMessageSink(ZmqMessagePtrStream in)
    {
        caf::aout(this) << "Building simple sink" << std::endl;

        return make_sink(
            in,
            // Initializer.
            [](ZmqMessagePtrSeq& xs) {
                // nop
            },
            // Consumer
                [&](ZmqMessagePtrSeq& xs, ZmqMessagePtr val) {
                std::string receivedMessage;
                receivedMessage.assign(val->data<char>(), val->size());
                caf::aout(this) << "MessageSink Received: " << receivedMessage << std::endl;
            },
                // Finalizer
                [&](ZmqMessagePtrSeq& xs, const caf::error& err) {
                if (err) {
                    caf::aout(this) << "MessageSink aborted with error: " << err << std::endl;
                }
                else {
                    caf::aout(this) << "MessageSink finalized after receiving: " << xs << std::endl;
                }
            }
        );
    }

    caf::make_sink_result<ZmqMultipartPtr> ZmqTextMessageSink::makeZmqMultipartSink(ZmqMultipartPtrStream in)
    {
        caf::aout(this) << "Building multipart sink" << std::endl;
        return make_sink(
            in,
            // Initializer.
            [](ZmqMultipartPtrSeq& xs) {
                // nop
            },
            // Consumer
                [&](ZmqMultipartPtrSeq& xs, ZmqMultipartPtr val) {
                std::string subscriptionTopic = val->popstr();
                caf::aout(this) << "Subscriber Topic: " << subscriptionTopic << std::endl;
                int frame = 0;
                while (!val->empty())
                {
                    std::string frameContent = val->popstr();
                    caf::aout(this) << "    Frame[" << ++frame << "]: " << frameContent << std::endl;
                }
            },
                // Finalizer
                [&](ZmqMultipartPtrSeq& xs, const caf::error& err) {
                if (err) {
                    caf::aout(this) << "MessageSink aborted with error: " << err << std::endl;
                }
                else {
                    caf::aout(this) << "MessageSink finalized after receiving: " << xs << std::endl;
                }
            }
        );
    }
}
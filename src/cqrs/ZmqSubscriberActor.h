#pragma once

#include <string>
#include <atomic>
#include <thread>
#include <caf/all.hpp>
#include <zmq.hpp>

namespace cqrs {

    // ZmqSubscriberActors need to become a stream source, then relevant 
    // message-unmarshallers/crackers become a stream-stage or stream-sink
    class ZmqSubscriberActor : public caf::event_based_actor
    {
    public:
        ZmqSubscriberActor(caf::actor_config& actorConfig, zmq::context_t& networkContext, const std::string& socketAddr, const std::string& subscrTopic);
        ~ZmqSubscriberActor() override;
    protected:
        caf::behavior make_behavior() override;

    private:
        zmq::socket_t subscriberSocket;
        std::atomic<bool> willContinuePolling;
        std::thread pollerThread;

        void pollSubscription();
        void pollMultipartSubscription();
        void onExit(caf::exit_msg& reason);
    };

}
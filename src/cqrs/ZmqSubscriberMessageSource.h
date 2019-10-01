#pragma once

#include <memory>
#include <caf/all.hpp>
#include <zmq.hpp>

namespace cqrs {

    /// This class unmarshals zmq messages of a zmq pub-sub subscription into a CAF actor system. 
    /// The resulting message must be pipelined to a CAF sink for processing 
    class ZmqSubscriberMessageSource : public caf::event_based_actor
    {
    public:
        ZmqSubscriberMessageSource(caf::actor_config& actorConfig, zmq::context_t& networkContext, const std::string& socketAddr, const std::string& subscrTopic);
        ~ZmqSubscriberMessageSource() override;

        struct StreamBuilder
        {
            ZmqSubscriberMessageSource& host;
            zmq::socket_t& subscriberSocket;
            std::thread pollerThread;
            std::atomic<bool> willContinuePolling;

            virtual ~StreamBuilder();
            virtual caf::behavior make_behavior() = 0;

            void join();
            void onExit();

        protected:
            StreamBuilder(ZmqSubscriberMessageSource& host, zmq::socket_t& subscriberSocket, const std::string& subscriptionTopic);
        };

    protected:
        caf::behavior make_behavior() override;

    private:
        using StreamBuilderPtr = std::unique_ptr<StreamBuilder>;
        StreamBuilderPtr streamBuilder;

        zmq::socket_t subscriberSocket;
        void onExit(caf::exit_msg& reason);
    };
}
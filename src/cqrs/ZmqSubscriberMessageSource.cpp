#include <vector>
#include <utility>
#include <functional>
#include <iostream>
#include "cqrs/ZmqReactor.h"
#include "cqrs/ZmqSubscriberMessageSource.h"
#include "cqrs/msg/Stream.h"
#include "cqrs/msg/ZmqMessagePtr.h"


namespace cqrs {

    template <typename ZmqMsgType>
    struct ZmqMessageStreamBuilder : ZmqSubscriberMessageSource::StreamBuilder
    {
        using MsgType = std::shared_ptr<ZmqMsgType>;
        using QueueType = std::deque<MsgType>;

        QueueType srcMessageBuffer;
        ZmqReactor reactor;

        ZmqMessageStreamBuilder(ZmqSubscriberMessageSource& host, zmq::socket_t& subscriberSocket, const std::string& subscriptionTopic)
            : ZmqSubscriberMessageSource::StreamBuilder(host, subscriberSocket, subscriptionTopic)
        {
            reactor.addReaction(subscriberSocket, std::bind(&ZmqMessageStreamBuilder::onMsgReceived, this, std::placeholders::_1), true);
            pollerThread = std::move(std::thread([&]() { while (willContinuePolling) reactor.poll(0); }));
        }

        virtual ~ZmqMessageStreamBuilder() {}

        void onMsgReceived(zmq::socket_ref s)
        {
            MsgType msg;
            ZmqMsgPtrTraits<MsgType>::recv(s, msg);
            caf::anon_send(&host, msg); // inject into the actor system
        }
        
        caf::behavior make_behavior() override
        {
            return {
                [&](MsgType msg)
                {
                    caf::aout(&host) << "buffering msg" << std::endl;
                    srcMessageBuffer.push_back(msg);
                },
                [&](msg::StartStream)
                {
                    return host.make_source(
                        [](caf::unit_t&) {},
                        [&](caf::unit_t&, caf::downstream<MsgType>& out, std::size_t num)
                        {
                            std::size_t count = srcMessageBuffer.size();
                            caf::aout(&host) << "Pushing " << count << " messages" << std::endl;
                            for (; count > 0; --count)
                            {
                                out.push(srcMessageBuffer.front());
                                srcMessageBuffer.pop_front();
                            }
                        },
                        [&](const caf::unit_t&) { return !willContinuePolling; }
                    );
                },
                [&](msg::EndStream) 
                {
                    caf::aout(&host) << "Received EndStream" << std::endl;
                    willContinuePolling = false;
                }
            };
        }
    };

    ZmqSubscriberMessageSource::ZmqSubscriberMessageSource(caf::actor_config& actorConfig, zmq::context_t& networkContext, const std::string& socketAddr, const std::string& subscrTopic)
        : caf::event_based_actor(actorConfig)
        , subscriberSocket(networkContext, zmq::socket_type::sub)
    {
        set_exit_handler(std::bind(&ZmqSubscriberMessageSource::onExit, this, std::placeholders::_1));

        subscriberSocket.connect(socketAddr);
        subscriberSocket.setsockopt(ZMQ_SUBSCRIBE, subscrTopic.c_str(), subscrTopic.size());

        streamBuilder = subscrTopic == std::string()
            ? StreamBuilderPtr(new ZmqMessageStreamBuilder<zmq::message_t>(*this, subscriberSocket, subscrTopic))
            : StreamBuilderPtr(new ZmqMessageStreamBuilder<zmq::multipart_t>(*this, subscriberSocket, subscrTopic));

        caf::aout(this) << "Built Subscription source for topic: " << subscrTopic << std::endl;
    }

    ZmqSubscriberMessageSource::~ZmqSubscriberMessageSource()
    {
        streamBuilder->join();
    }

    caf::behavior ZmqSubscriberMessageSource::make_behavior()
    {
        return streamBuilder->make_behavior();
    }

    void ZmqSubscriberMessageSource::onExit(caf::exit_msg& reason)
    {
        caf::aout(this) << "Received onExit" << std::endl;

        streamBuilder->onExit();
    }

    ZmqSubscriberMessageSource::StreamBuilder::StreamBuilder(ZmqSubscriberMessageSource& host, zmq::socket_t& subscriberSocket, const std::string& subscriptionTopic)
        : host(host)
        , subscriberSocket(subscriberSocket)
        , willContinuePolling(true)
    {}

    ZmqSubscriberMessageSource::StreamBuilder::~StreamBuilder()
    {}

    void ZmqSubscriberMessageSource::StreamBuilder::join()
    {
        pollerThread.join();
    }

    void ZmqSubscriberMessageSource::StreamBuilder::onExit()
    {
        willContinuePolling = false;
    }
}

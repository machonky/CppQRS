#include <vector>
#include <utility>
#include <functional>
#include <iostream>
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

        ZmqMessageStreamBuilder(ZmqSubscriberMessageSource& host, zmq::socket_t& subscriberSocket, const std::string& subscriptionTopic)
            : ZmqSubscriberMessageSource::StreamBuilder(host, subscriberSocket, subscriptionTopic)
        {
            using namespace std;
            pollerThread = move(thread(bind(&ZmqMessageStreamBuilder::pollSubscription, this)));
        }

        virtual ~ZmqMessageStreamBuilder() {}
        
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

        void pollSubscription() 
        {
            //caf::aout(&host) << "Starting poller polling:" << (bool)willContinuePolling << std::endl;
            MsgType msg;
            std::vector<zmq::pollitem_t> pollItems = { {subscriberSocket, 0, ZMQ_POLLIN, 0} };
            while (willContinuePolling)
            {
                if (zmq::poll(pollItems.data(), 1, 0) == 1)
                {
                    if (pollItems[0].revents & ZMQ_POLLIN)
                    {
                        MsgType msg;
                        ZmqMsgPtrTraits<MsgType>::recv(subscriberSocket, msg);
                        caf::anon_send(&host, msg); // inject into the actor system
                        //caf::aout(&host) << "Received zmq msg" << std::endl;
                    }
                }
            }
            //caf::aout(&host) << "Finishing poller polling:" << (bool)willContinuePolling << std::endl;
        }
    };

    ZmqSubscriberMessageSource::ZmqSubscriberMessageSource(caf::actor_config& actorConfig, zmq::context_t& networkContext, const std::string& socketAddr, const std::string& subscrTopic)
        : caf::event_based_actor(actorConfig)
        , subscriberSocket(networkContext, zmq::socket_type::sub)
    {
        using namespace std;
        set_exit_handler(bind(&ZmqSubscriberMessageSource::onExit, this, placeholders::_1));

        subscriberSocket.connect(socketAddr);
        subscriberSocket.setsockopt(ZMQ_SUBSCRIBE, subscrTopic.c_str(), subscrTopic.size());

        streamBuilder = subscrTopic == string()
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

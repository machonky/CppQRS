#include "ZmqSubscriberActor.h"
#include "ZmqSubscriberActor.h"
#include <vector>
#include <functional>
#include "cqrs/ZmqSubscriberActor.h"
#include "cqrs/msg/ZmqMessagePtr.h"

namespace cqrs {

    ZmqSubscriberActor::ZmqSubscriberActor(caf::actor_config& actorConfig, zmq::context_t& networkContext, const std::string& socketAddr, const std::string& subscrTopic)
        : caf::event_based_actor(actorConfig)
        , subscriberSocket(networkContext, zmq::socket_type::sub)
        , willContinuePolling(true)
    {
        using namespace std;

        set_exit_handler(bind(&ZmqSubscriberActor::onExit, this, placeholders::_1));

        subscriberSocket.connect(socketAddr);
        subscriberSocket.setsockopt(ZMQ_SUBSCRIBE, subscrTopic.c_str(), subscrTopic.size());

        pollerThread = subscrTopic == string()
            ? move(thread(bind(&ZmqSubscriberActor::pollSubscription, this)))
            : move(thread(bind(&ZmqSubscriberActor::pollMultipartSubscription, this)));
    }

    ZmqSubscriberActor::~ZmqSubscriberActor()
    {
        pollerThread.join();
    }

    caf::behavior ZmqSubscriberActor::make_behavior()
    {
        return caf::message_handler
        {
            [&](ZmqMessagePtr msg) {
                std::string receivedMessage;
                receivedMessage.assign(msg->data<char>(), msg->size());
                caf::aout(this) << "Subscriber Received: " << receivedMessage << std::endl;
            },
            [&](ZmqMultipartPtr msg) {
                std::string subscriptionTopic = msg->popstr();
                caf::aout(this) << "Subscriber Topic: " << subscriptionTopic << std::endl;
                int frame = 0;
                while (!msg->empty())
                {
                    std::string frameContent = msg->popstr();
                    caf::aout(this) << "    Frame[" << ++frame << "]: " << frameContent << std::endl;
                }
            }
        };
    }

    void ZmqSubscriberActor::pollSubscription()
    {
        ZmqMessagePtr msg;
        std::vector<zmq::pollitem_t> pollItems = { {subscriberSocket, 0, ZMQ_POLLIN, 0} };
        while (willContinuePolling)
        {
            if (zmq::poll(pollItems.data(), 1, 0) == 1)
            {
                if (pollItems[0].revents & ZMQ_POLLIN)
                {
                    msg = makeZmqMessagePtr();
                    subscriberSocket.recv(*msg);
                    send(this, msg);
                }
            }
        }
    }

    void ZmqSubscriberActor::pollMultipartSubscription()
    {
        ZmqMultipartPtr msg;
        std::vector<zmq::pollitem_t> pollItems = { {subscriberSocket, 0, ZMQ_POLLIN, 0} };
        while (willContinuePolling)
        {
            if (zmq::poll(pollItems.data(), 1, 0) == 1)
            {
                if (pollItems[0].revents & ZMQ_POLLIN)
                {
                    msg = makeZmqMultipartPtr();
                    msg->recv(subscriberSocket);
                    send(this, msg);
                }
            }
        }
    }

    void ZmqSubscriberActor::onExit(caf::exit_msg& reason)
    {
        willContinuePolling = false;
    }
}
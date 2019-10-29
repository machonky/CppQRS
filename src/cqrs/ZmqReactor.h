#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <memory>
#include <chrono>

#include <zmq.hpp>
#undef min // hack!
#undef max // hack!


namespace cqrs {

    class ZmqReactor
    {
        ZmqReactor(const ZmqReactor&) = delete;
        ZmqReactor& operator=(const ZmqReactor&) = delete;

    public:
        using SocketCallback = std::function<void(zmq::socket_ref)>;

        ZmqReactor() {}

        void addReaction(zmq::socket_t& socket, SocketCallback callback, bool initPollItems = false);
        void deactivate(zmq::socket_ref s);

        void init();

        int poll(std::chrono::milliseconds timeout);

    private:
        struct Reaction
        {
            zmq::pollitem_t pollitem;
            SocketCallback react;
            std::atomic_bool active;

            Reaction(zmq::pollitem_t pollitem, SocketCallback react, bool active)
                : pollitem(pollitem)
                , react(react)
                , active(active)
            {}
        };

        using ReactionPtr = std::unique_ptr<Reaction>;
        using ReactionMap = std::unordered_map<zmq::socket_ref, ReactionPtr>;
        using ReactionSeq = std::vector<Reaction*>;
        using PollItemSeq = std::vector<zmq::pollitem_t>;
        using SocketSeq = std::vector<zmq::socket_ref>;

        PollItemSeq pollItems;
        ReactionMap reactions;
    };
}
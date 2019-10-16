#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <zmq.hpp>

namespace cqrs {

    class ZmqReactor
    {
        ZmqReactor(const ZmqReactor&) = delete;
        ZmqReactor& operator=(const ZmqReactor&) = delete;

    public:
        using SocketCallback = std::function<void(zmq::socket_ref)>;

        ZmqReactor() {}

        void addReaction(zmq::socket_t& socket, SocketCallback callback);
        int poll(long timeout);

    private:
        struct Reaction
        {
            zmq::pollitem_t pollitem;
            SocketCallback react;
        };

        using ReactionMap = std::unordered_multimap<zmq::socket_ref, Reaction>;
        using PollItemSeq = std::vector<zmq::pollitem_t>;

        PollItemSeq pollItems;
        ReactionMap reactions;
    };
}
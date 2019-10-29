#include <algorithm>
#include <utility>
#include <iostream>
#include "cqrs/ZmqReactor.h"

namespace cqrs {

    void ZmqReactor::addReaction(zmq::socket_t& socket, SocketCallback callback, bool initPollItems)
    {
        zmq::socket_ref handle(socket);
        zmq::pollitem_t pollItem{ socket.handle(), 0, ZMQ_POLLIN, 0 };
        ReactionPtr reaction = std::make_unique<Reaction>(pollItem, callback, true);
        reactions.insert(std::move(std::make_pair(handle, std::move(reaction))));
        if (initPollItems) init();
    }

    void ZmqReactor::deactivate(zmq::socket_ref s)
    {
        auto it = reactions.find(s);
        if (it != reactions.end())
        {
            auto& reaction = *(*it).second;
            reaction.active = false;
        }
    }

    void ZmqReactor::init()
    {
        pollItems.clear();
        SocketSeq purgeItems;
        for (auto&& kvp : reactions)
        {
            if (kvp.second->active)
            {
                pollItems.push_back(kvp.second->pollitem);
            }
            else
            {
                purgeItems.push_back(kvp.first);
            }
        }

        for (auto&& s : purgeItems)
        {
            reactions.erase(s);
        }
    }
    
    int ZmqReactor::poll(std::chrono::milliseconds timeout)
    {
        int result = zmq::poll(pollItems, timeout);
        if (result > 0)
        {
            for(auto&& item : pollItems)
            {
                zmq::socket_ref handle(zmq::from_handle, item.socket);
                auto& reaction = reactions[handle];
                if (reaction->active)
                {
                    if (item.revents & ZMQ_POLLIN)
                    {
                        reaction->react(handle);
                    }
                }
            }
        }
        return result;
    }
}
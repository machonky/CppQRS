#include <algorithm>
#include <utility>
#include "cqrs/ZmqReactor.h"

namespace cqrs {

    void ZmqReactor::addReaction(zmq::socket_t& socket, SocketCallback callback)
    {
        zmq::socket_ref handle(socket);
        Reaction reaction = { { socket.handle(), 0, ZMQ_POLLIN, 0 }, callback };
        pollItems.push_back(reaction.pollitem);
        reactions.insert(std::make_pair(handle, std::move(reaction)));
    }
    
    int ZmqReactor::poll(long timeout)
    {
        int result = zmq::poll(pollItems, timeout);
        if (result > 0)
        {
            for (auto&& item : pollItems)
            {
                if (item.revents & ZMQ_POLLIN)
                {
                    zmq::socket_ref handle(zmq::from_handle, item.socket);
                    auto range = reactions.equal_range(handle);
                    std::for_each(range.first, range.second, [&](ReactionMap::value_type& kvp)
                    {
                        auto& reaction = kvp.second;
                        reaction.react(handle);
                    });
                }
            }
        }
        return result;
    }
}
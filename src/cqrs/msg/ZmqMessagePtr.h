#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <caf/all.hpp>
#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace cqrs {

    template <typename MsgPtrType>
    struct ZmqMsgPtrTraits {};

    // zmq::message_t won't work nicely with CAF at all, but can be passed around easily by shared_ptr
    using ZmqMessagePtr = std::shared_ptr<zmq::message_t>;
    using ZmqMessagePtrStream = caf::stream<ZmqMessagePtr>;
    using ZmqMessagePtrSeq = std::vector<ZmqMessagePtr>;
    using ZmqMessagePtrQueue = std::deque<ZmqMessagePtr>;

    template <class... CtorParamTypes>
    inline ZmqMessagePtr makeZmqMessagePtr(CtorParamTypes&& ... args)
    {
        return std::make_shared<zmq::message_t>(std::forward<CtorParamTypes>(args)...);
    }

    template <>
    struct ZmqMsgPtrTraits<ZmqMessagePtr> {
        static void recv(zmq::socket_t& socket, ZmqMessagePtr& msg) 
        { 
            msg = makeZmqMessagePtr();
            socket.recv(*msg); 
        }
    };

    using ZmqMultipartPtr = std::shared_ptr<zmq::multipart_t>;
    using ZmqMultipartPtrStream = caf::stream<ZmqMultipartPtr>;
    using ZmqMultipartPtrSeq = std::vector<ZmqMultipartPtr>;
    using ZmqMultipartPtrQueue = std::deque<ZmqMultipartPtr>;

    template <class... CtorParamTypes>
    inline ZmqMultipartPtr makeZmqMultipartPtr(CtorParamTypes&& ... args)
    {
        return std::make_shared<zmq::multipart_t>(std::forward<CtorParamTypes>(args)...);
    }

    template <>
    struct ZmqMsgPtrTraits<ZmqMultipartPtr> 
    {
        static void recv(zmq::socket_t& socket, ZmqMultipartPtr& msg)
        {
            msg = makeZmqMultipartPtr();
            msg->recv(socket);
        }
    };
}

CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMessagePtr)
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMessagePtrSeq) // Streaming workaround
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMultipartPtr)
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMultipartPtrSeq) // Streaming workaround

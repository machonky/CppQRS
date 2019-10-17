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
    struct ZmqMsgPtrTraits<ZmqMessagePtr> 
    {
        static void recv(zmq::detail::socket_base& socket, ZmqMessagePtr& msg) 
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

    inline bool recv_multi(zmq::multipart_t& multiMsg, zmq::detail::socket_base& socket, int flags = 0)
    {
        multiMsg.clear();
        bool more = true;
        while (more) {
            zmq::message_t message;
#ifdef ZMQ_CPP11
            if (!socket.recv(message, static_cast<zmq::recv_flags>(flags)))
                return false;
#else
            if (!socket.recv(&message, flags))
                return false;
#endif
            more = message.more();
            multiMsg.add(std::move(message));
        }
        return true;
    }

    template <>
    struct ZmqMsgPtrTraits<ZmqMultipartPtr> 
    {
        static void recv(zmq::detail::socket_base& socket, ZmqMultipartPtr& msg)
        {
            msg = makeZmqMultipartPtr();
            recv_multi(*msg, socket);
        }
    };
}

CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMessagePtr)
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMessagePtrSeq) // Streaming workaround
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMultipartPtr)
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMultipartPtrSeq) // Streaming workaround

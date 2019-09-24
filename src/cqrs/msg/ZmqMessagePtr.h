#pragma once

#include <memory>
#include <caf/all.hpp>
#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace cqrs {

    // zmq::message_t won't work nicely with CAF at all, but can be passed around easily by shared_ptr
    typedef std::shared_ptr<zmq::message_t> ZmqMessagePtr;

    template <class... CtorParamTypes>
    inline ZmqMessagePtr makeZmqMessagePtr(CtorParamTypes&& ... args)
    {
        return std::make_shared<zmq::message_t>(std::forward<CtorParamTypes>(args)...);
    }

    typedef std::shared_ptr<zmq::multipart_t> ZmqMultipartPtr;

    template <class... CtorParamTypes>
    inline ZmqMultipartPtr makeZmqMultipartPtr(CtorParamTypes&& ... args)
    {
        return std::make_shared<zmq::multipart_t>(std::forward<CtorParamTypes>(args)...);
    }

}

CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMessagePtr)
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMultipartPtr)

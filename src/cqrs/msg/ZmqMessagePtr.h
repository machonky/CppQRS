#pragma once

#include <memory>
//#include <type_traits>
#include <caf/all.hpp>
#include <zmq.hpp>

namespace cqrs {

    // zmq::message_t won't work nicely with CAF at all, but can be passed around easily by shared_ptr
    typedef std::shared_ptr<zmq::message_t> ZmqMessagePtr;

    template <class... CtorParamTypes>
    inline ZmqMessagePtr makeZmqMessagePtr(CtorParamTypes&&... args) 
    {
        return std::make_shared<zmq::message_t>(std::forward<CtorParamTypes>(args)...);
    }

}

CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::ZmqMessagePtr)

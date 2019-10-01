#pragma once

#include <caf/all.hpp>

namespace cqrs { namespace msg {

    struct StartStream {};

    struct EndStream {};

} }

CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::msg::StartStream)
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::msg::EndStream)

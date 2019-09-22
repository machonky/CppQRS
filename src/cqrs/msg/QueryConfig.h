#pragma once

#include <caf/all.hpp>

namespace cqrs { namespace msg {

    struct QueryConfig {};

} }

CAF_ALLOW_UNSAFE_MESSAGE_TYPE(::cqrs::msg::QueryConfig)

#pragma once

#include "logger.hpp"

#define debug_msg(...) \
    do { if constexpr (debug_mode) logging::debug().msg(__VA_ARGS__); } while(0)

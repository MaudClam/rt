#pragma once
#include <string_view>
#include <sstream>
#include <string>
#include <chrono>
#include <cstdint>


namespace common {

using sv_t    = std::string_view;
using os_t    = std::ostream;
using oss_t   = std::ostringstream;
using delay_t = std::chrono::microseconds;


template<class T>
struct DelayConfig {
    static_assert(sizeof(T) == 0,
      "DelayConfig: there is no specialization for this type chrono::duration");
};

template<>
struct DelayConfig<std::chrono::microseconds> {
    static constexpr auto suffix = "us";
};

inline constexpr auto delay_suffix = DelayConfig<delay_t>::suffix;

enum class IoStatus : uint8_t { Ok, Contended, Failed };


}// namespace common


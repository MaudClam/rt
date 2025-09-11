#pragma once
#include "types.hpp"

namespace common {

struct Return {
    sv_t status = "";
    sv_t prompt = "";
    bool fatal  = false;

    Return& set_prompt(sv_t p) noexcept { prompt = p; return *this; }
    Return& fatal_on() noexcept { fatal = true; return *this; }
    Return& fatal_off() noexcept { fatal = false; return *this; }
    [[nodiscard]] bool is_fatal() const noexcept { return fatal; }
    [[nodiscard]] bool ok() const noexcept { return status.empty(); }
    explicit operator bool() const noexcept { return !ok(); }
    os_t& write(os_t& os) const {
        os << status;
        if (!prompt.empty())
            os << " '" << prompt << '\'';
        return os;
    }
};

inline os_t& operator<<(os_t& os, const Return& ret) { return ret.write(os); }

[[nodiscard]] inline Return ok() noexcept { return {}; }

[[nodiscard]] inline
Return error(sv_t status, sv_t prompt = {}, bool fatal = false) noexcept {
#if !defined(NDEBUG)
        if (status.empty()) status = "CODING ERROR (missing error status)";
#elif
        if (status.empty()) status = "Unknown error";
#endif
    return {status, prompt, fatal};
}

}//namespace common

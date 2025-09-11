#pragma once
#include "types.hpp"
#include "buffer.hpp"

#include <cstddef>
#include <cerrno>

namespace common {

template <size_t N> inline
void append_io_prefix_failed_errno(RawBuffer<N>& buf, int err_no) noexcept {
    buf.add_prefix("┌─ ", "[io:failed errno=", to_sv(err_no), " raw] ");
}

#if defined(_WIN32)
#include <windows.h>
template <size_t N> inline
void append_io_prefix_failed_gle(RawBuffer<N>& buf, unsigned gle) noexcept {
    buf.add_prefix("┌─ ", "[io:failed gle=", to_sv((int)gle), " raw] ");
}
#endif

template <size_t N> inline
void append_io_prefix_contended(RawBuffer<N>& buf,
                                       int attempts, delay_t delay) noexcept {
    buf.add_prefix("┌─ ",
                   "[io:contended attempts=", attempts,
                   " wait=", (int)(attempts*delay.count()), delay_suffix,
                   " raw] ");
}

template <size_t N> inline
void append_io_prefix_failure(RawBuffer<N>& buf,
                     IoStatus io_status, int attempts, delay_t delay) noexcept {
    if (io_status == IoStatus::Contended) {
        append_io_prefix_contended(buf, attempts, delay);
    } else if (io_status == IoStatus::Failed) {
#if defined(_WIN32)
        append_io_prefix_failed_gle(buf, (unsigned)::GetLastError());
#else
        append_io_prefix_failed_errno(buf, errno);
#endif
    }
}

} // namespace common

#pragma once
#if defined(NDEBUG)
 constexpr bool debug_mode = false;
#else
 constexpr bool debug_mode = true;
#endif
#include <iostream>
#include <thread>
#include <functional>
#include <utility>
#include <cstddef>
#include <cstdint>
#include "types.hpp"
#include "traits.hpp"
#include "raw_io.hpp"
#include "buffer.hpp"
#include "return.hpp"
#include "error_prefix.hpp"

namespace common {

inline std::timed_mutex  stdout_mutex{};
inline std::timed_mutex  stderr_mutex{};
inline constexpr delay_t kIoDelay = delay_t{100};
inline constexpr int     kIoAttempts = 3;
inline constexpr size_t  kIoFallbackBufferSize = 1024;

#if !defined(NDEBUG)
inline std::atomic<uint64_t> stderr_contended{0};
inline std::atomic<uint64_t> stdout_contended{0};
inline std::atomic<uint64_t> stderr_failed{0};
inline std::atomic<uint64_t> stdout_failed{0};
inline thread_local int      tl_last_stderr_attempts{0};
inline thread_local int      tl_last_stdout_attempts{0};
[[nodiscard]] inline bool is_metrics() noexcept {
    const auto sc = val(stderr_contended);
    const auto so = val(stdout_contended);
    const auto sf = val(stderr_failed);
    const auto fo = val(stdout_failed);
    return (sc | so | sf | fo) != 0;
}
inline void reset_metrics() noexcept {
    stderr_contended.store(0, std::memory_order_relaxed);
    stdout_contended.store(0, std::memory_order_relaxed);
    stderr_failed.store(0, std::memory_order_relaxed);
    stdout_failed.store(0, std::memory_order_relaxed);
}
inline void flush_metrics_dump() noexcept {
    const auto sc = val(stderr_contended);
    const auto so = val(stdout_contended);
    const auto sf = val(stderr_failed);
    const auto fo = val(stdout_failed);
    if ((sc | so | sf | fo) == 0) [[likely]] {
        sv_t msg{"\nMETRICS DUMP: Metrics have no changes.\n"};
        raw_write_stderr(msg.data(), msg.size());
        return;
    }
    RawBuffer<512>
    buf(
        "\nMETRICS DUMP ===============\n",
        "stderr_contended: ", to_sv(sc), '\n',
        "stdout_contended: ", to_sv(so), '\n',
        "stderr_failed:    ", to_sv(sf), '\n',
        "stdout_failed:    ", to_sv(fo), '\n',
        "METRICS DUMP END ===========\n"
        );
    buf.finalize_ellipsis_newline(true);
    raw_write_stderr(buf.data(), buf.size());
    reset_metrics();
}
#endif

enum class ExitCode : uint8_t {
    Success        = EXIT_SUCCESS,
    UnknownError   = EXIT_FAILURE,
    OutputFailure  = 2,
    CfgFileFailure = 3,
    CmdlineFailure = 4,
    LoggingFailure = 5,
};

enum class ExitPolicy : uint8_t {
    None,
    ExitOnFatal,
    ExitOnIoFailure,
    ExitOnEither
};

[[noreturn]] inline void fatal_exit(ExitCode code) noexcept {
    std::exit(static_cast<int>(code));
}

[[noreturn]] inline void graceful_exit() noexcept {
    std::exit(static_cast<int>(ExitCode::Success));
}

template<typename... X>
constexpr auto stream_inserter() {
    return [](os_t& os, const X&... xs) {
        if constexpr (sizeof...(X) > 0)
            (os << ... << xs);
    };
}

template<typename F, class... Args>
requires (traits::OsWriter<F, Args...>)
[[nodiscard]] inline IoStatus stream_locker(os_t& os, std::timed_mutex& mtx,
                           delay_t delay, F&& writer, Args&&... args) noexcept {
bool ok = os.good();
try {
//        throw 42;//FIXME: 1
    std::unique_lock lk(mtx, std::defer_lock);
    if (!lk.try_lock_for(delay)) return IoStatus::Contended;
    std::invoke(std::forward<F>(writer), os, std::forward<Args>(args)...);
    os << std::flush;
    ok = os.good();
} catch (...) {
    ok = false;
}
return ok ? IoStatus::Ok : IoStatus::Failed;
}

template<typename... Args>
[[nodiscard]] inline IoStatus with_stdout(const Args&... args) noexcept {
    IoStatus io_status = IoStatus::Contended;
    int attempts = 0;
    while (attempts < kIoAttempts) {
        ++attempts;
        io_status = stream_locker(std::cout, stdout_mutex, kIoDelay,
                                  stream_inserter<Args...>(), args...);
        if (io_status == IoStatus::Ok) [[likely]] break;
        if (io_status == IoStatus::Failed) [[unlikely]] break;
        std::this_thread::sleep_for(kIoDelay);
    }
#if !defined(NDEBUG)
    tl_last_stdout_attempts = attempts;
    if (io_status == IoStatus::Contended)
        stdout_contended.fetch_add(1, std::memory_order_relaxed);
    if (io_status == IoStatus::Failed)
        stdout_failed.fetch_add(1, std::memory_order_relaxed);
#endif
    return io_status;
}

template<typename... Args>
[[nodiscard]] inline IoStatus with_stderr(const Args&... args) noexcept {
    IoStatus io_status = IoStatus::Contended;
    int attempts = 0;
    while (attempts < kIoAttempts) {
        ++attempts;
        io_status = stream_locker(std::cerr, stderr_mutex, kIoDelay,
                                  stream_inserter<Args...>(), args...);
        if (io_status == IoStatus::Ok) break;
        if (io_status == IoStatus::Failed) break;
        std::this_thread::sleep_for(kIoDelay);
    }
#if !defined(NDEBUG)
    tl_last_stderr_attempts = attempts;
    if (io_status == IoStatus::Contended)
        stderr_contended.fetch_add(attempts, std::memory_order_relaxed);
    if (io_status == IoStatus::Failed)
        stderr_failed.fetch_add(attempts, std::memory_order_relaxed);
#endif
    return io_status;
}

inline void raw_write_stdout(sv_t sv) noexcept {
    raw_write_stdout(sv.data(), sv.size());
}

template<size_t N>
inline void raw_write_stdout(const RawBuffer<N>& buf) noexcept {
    raw_write_stdout(buf.view());
}

inline void raw_write_stderr(sv_t sv) noexcept {
    raw_write_stderr(sv.data(), sv.size());
}

template<size_t N>
inline void raw_write_stderr(const RawBuffer<N>& buf) noexcept {
    raw_write_stderr(buf.view());
}

[[nodiscard]] inline
IoStatus report_error(sv_t ctx, const Return& ret, sv_t help = {}) noexcept {
    if (ret.ok()) return IoStatus::Ok;
    sv_t sep = ctx.empty() ? sv_t{} : " ";
    IoStatus io_status = with_stderr(ctx, sep, ret, '\n', help);
    if (io_status == IoStatus::Ok) return IoStatus::Ok;
    RawBuffer<kIoFallbackBufferSize> buf(ctx, sep, ret, '\n', help);
#if !defined(NDEBUG)
    append_io_prefix_failure(buf, io_status, tl_last_stderr_attempts, kIoDelay);
#endif
    raw_write_stderr(buf);
    return io_status;
}

inline IoStatus
handle_error(sv_t context, const Return& status,
             ExitCode logic_code = ExitCode::UnknownError, sv_t help = {},
             ExitPolicy policy = ExitPolicy::ExitOnEither) noexcept {
    if (status.ok()) return IoStatus::Ok;
    const IoStatus io_status   = report_error(context, status, help);
    const ExitCode io_code     = ExitCode::OutputFailure;
    const bool     io_fatal    = (io_status == IoStatus::Failed);
    const bool     logic_fatal = status.is_fatal();
    using enum ExitPolicy;
    switch (policy) {
        case None:            break;
        case ExitOnFatal:     if (logic_fatal) fatal_exit(logic_code); break;
        case ExitOnIoFailure: if (io_fatal)    fatal_exit(io_code); break;
        case ExitOnEither: {  if (io_fatal)    fatal_exit(io_code);
                              if (logic_fatal) fatal_exit(logic_code);
                              break; }
    }
    return io_status;
}

} // namespace common

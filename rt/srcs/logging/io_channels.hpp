#pragma once
#include <cstdint>
#include <ostream>
#include <string_view>
#include <filesystem>
#include <fstream>
#include "terminal_width.hpp"



namespace logging::io {

using os_t    = std::ostream;
using sv_t    = std::string_view;
using ofs_t   = std::ofstream;
using ifs_t   = std::ifstream;
using flags_t = uint8_t;
namespace fs  = std::filesystem;

enum class Output : flags_t {
    Stdout     = 0b0000'0000,
    Stderr     = 0b0010'0000,
    File       = 0b0100'0000,
    Buffer     = 0b1000'0000,

    Append     = 0b0000'0001,
    Indexing   = 0b0000'0010,
    TimeIndex  = 0b0000'0100,
    CreateDirs = 0b0000'1000
};

constexpr Output ChannelMask = static_cast<Output>(0b1110'0000);
constexpr Output PolicyMask  = static_cast<Output>(0b0001'1111);

[[nodiscard]]
inline constexpr Output operator|(Output a, Output b) noexcept {
    return static_cast<Output>(
        static_cast<flags_t>(a) | static_cast<flags_t>(b));
}

inline constexpr Output operator&(Output a, Output b) noexcept {
    return static_cast<Output>(
        static_cast<flags_t>(a) & static_cast<flags_t>(b));
}

[[nodiscard]] inline constexpr flags_t raw(Output v) noexcept {
    return static_cast<flags_t>(v);
}

[[nodiscard]] inline constexpr Output channel_of(Output v) noexcept {
    return static_cast<Output>(raw(v) & raw(ChannelMask));
}

[[nodiscard]] inline constexpr bool has_flag(Output v, Output f) noexcept {
    return static_cast<flags_t>(v & f) == static_cast<flags_t>(f);
}

[[nodiscard]] inline constexpr bool is_stdout(Output v) noexcept {
    return (raw(v) & raw(ChannelMask)) == 0;      // Stdout = 0
}

[[nodiscard]] inline constexpr bool is_stderr(Output v) noexcept {
    return has_flag(v, Output::Stderr);
}

[[nodiscard]] inline constexpr bool is_file(Output v) noexcept {
    return has_flag(v, Output::File);
}

[[nodiscard]] inline constexpr bool is_buffer(Output v) noexcept {
    return has_flag(v, Output::Buffer);
}

[[nodiscard]] inline constexpr bool has_single_channel(Output v) noexcept {
    const flags_t c = raw(channel_of(v));
    return c == 0 || (c & (c - 1)) == 0;
}

[[nodiscard]] inline constexpr bool out_supports_tty(Output v) noexcept {
    return is_stdout(v) || is_stderr(v);
}

inline os_t& operator<<(os_t& os, Output v) {
    if (is_file(v)) {                        os << "File";
        if (has_flag(v, Output::Append))     os << "|Append";
        if (has_flag(v, Output::Indexing))   os << "|Indexing";
        if (has_flag(v, Output::TimeIndex))  os << "|TimeIndex";
        if (has_flag(v, Output::CreateDirs)) os << "|CreateDirs";
    } else if (is_stderr(v)) {               os << "Stderr";
    } else if (is_buffer(v)) {               os << "Buffer";
    } else {                                 os << "Stdout";
    }
    return os;
}

[[nodiscard]] inline
sv_t format_timestamp_suffix() noexcept {
    static thread_local char buf[32];
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_MSC_VER)
    localtime_s(&tm, &tt);
#elif defined(__unix__) || defined(__APPLE__)
    localtime_r(&tt, &tm);
#else
    if (auto* tmp = std::localtime(&tt)) tm = *tmp;
#endif
    const size_t n = std::strftime(buf, sizeof(buf), "_%Y-%m-%d_%H.%M.%S", &tm);
    return {buf, n};
}

[[nodiscard]] inline
fs::path indexed_candidate(const fs::path& base, const fs::path& ext, int index) {
    fs::path candidate;
    candidate = base;
    candidate += "(";
    candidate += common::to_sv(index);
    candidate += ")";
    candidate += ext;
    return candidate;
}

[[nodiscard]] inline
fs::path base_with_timestamp(const fs::path& base) {
    fs::path result;
    result = base;
    result += format_timestamp_suffix();
    return result;
}

[[nodiscard]] inline std::optional<fs::path>
on_collision(const fs::path& base_with_ts, const fs::path& ext)  {
    std::error_code ec;
    fs::path candidate;
    for (int i = 0; i < kIntMax; ++i) {
        if (i == 0) {
            candidate = base_with_ts;
            candidate += ext;
        } else {
            candidate = indexed_candidate(base_with_ts, ext, i);
        }
        if (candidate.empty()) return std::nullopt;
        if (!fs::exists(candidate, ec)) return candidate;
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<fs::path>
prepare_output_file_path(const fs::path& path,
                                          Output mode = Output::File) noexcept {
    if (!is_file(mode)) return std::nullopt;
    try {
        fs::path dir  = path.parent_path();
        fs::path base = path.stem();
        fs::path ext  = path.extension();
        std::error_code ec;
        if (!dir.empty() && !fs::exists(dir, ec)) {
            if (has_flag(mode, Output::CreateDirs)) {
                fs::create_directories(dir, ec);
                if (ec || !fs::exists(dir, ec)) return std::nullopt;
            } else {
                return std::nullopt;
            }
        }
        fs::path stamped = has_flag(mode, Output::TimeIndex)
                         ? base_with_timestamp(dir / base)
                         : dir / base;
        if (stamped.empty())                  return std::nullopt;
        if (has_flag(mode, Output::Indexing)) return on_collision(stamped, ext);
        fs::path candidate = stamped; candidate += ext;
        if (has_flag(mode, Output::Append))   return candidate;
        if (!fs::exists(candidate, ec))       return candidate;
        return std::nullopt;
    } catch (...) { return std::nullopt; }
}

[[nodiscard]] inline std::optional<fs::path>
prepare_output_file_path(sv_t raw_path, Output mode = Output::File) noexcept {
    try {
        std::error_code ec;
        auto path = fs::weakly_canonical(fs::current_path() / raw_path, ec);
        return prepare_output_file_path(path, mode);
    } catch (...) {
        return std::nullopt;
    }
}

[[nodiscard]] inline
common::Return open_output_file(ofs_t& out, sv_t raw_path,
                                          Output mode = Output::File) noexcept {
    using namespace logging;
    using namespace common;
    if (!has_flag(mode, Output::File))
        return error("Output mode does not target a file");
    auto path = prepare_output_file_path(raw_path, mode);
    if (!path)
        return error("Invalid or inaccessible file path", raw_path);
    auto flags = std::ios::out;
    if (has_flag(mode, Output::Append))
        flags |= std::ios::app;
    else
        flags |= std::ios::trunc;
    out.open(*path, flags);
    if (!out.is_open()) {
        if (!fs::exists(path->parent_path()))
            return error("Parent directory does not exist",
                                                  tl_copy(path->parent_path()));
        if (!fs::is_regular_file(*path) && fs::exists(*path))
            return error("Not a regular file", tl_copy(*path));
        return error("Failed to open file (permission denied or I/O error)",
                                                                tl_copy(*path));
    }
    return ok();
}

[[nodiscard]] inline
common::Return open_input_file(ifs_t& in, const fs::path& path) noexcept {
    using namespace common;
    if (path.empty()) return error("empty file path");
    std::error_code ec;
    if (!fs::exists(path, ec))
        return error("file does not exist", tl_copy(path));
    if (!fs::is_regular_file(path, ec))
        return error("not a regular file", tl_copy(path));
    in.open(path);
    if (!in.is_open())
        return error("failed to open file (permission denied or I/O error)",
                                                                 tl_copy(path));
    return ok();
}

[[nodiscard]] inline
common::Return open_input_file(ifs_t& in, sv_t raw_path) noexcept {
    using namespace common;
    if (raw_path.empty()) return error("empty file path");
    fs::path path;
    try {
        path = fs::path{raw_path};
    } catch (...) {
        return error("invalid path encoding or allocation failure", raw_path);
    }
    return open_input_file(in, path);
}

} // namespace logging::io


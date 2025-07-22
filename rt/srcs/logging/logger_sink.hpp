#pragma once
#include <string_view>
#include <ostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include "traits.hpp"
#include "logging_utils.hpp"
#include "../config.hpp"

namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;
using ofs_t = std::ofstream;

using Flags             = rt::LoggerStatusFlags;
using LogConfig         = rt::Config;
inline auto& log_config = rt::config;

class LoggerSink {
public:
    explicit LoggerSink(rt::Output mode, sv_t raw_path = {},
                                         bool fatal_on_failure = false) noexcept
    : mode_(mode),
      raw_path_(raw_path),
      fatal_on_failure_(fatal_on_failure)
    {
        using namespace rt;
        rt::Return status = init();
        status.write_error_if(std::cerr, "LoggerSink");
        if (!status.ok() && fatal_on_failure_) {
            write_logger_warns(std::cerr, cfg_.logger_flags);
            fatal_exit(rt::ExitCode::OutputFailure);
        }
    }

    ~LoggerSink() noexcept {
        using namespace rt;
        flush();
        restore_output();
        if (cfg_.warns_allowed && cfg_.logger_flags != 0) {
            write_logger_warns(out_ ? *out_ : std::cerr, cfg_.logger_flags);
            cfg_.logger_flags = static_cast<flags_t>(LoggerStatusFlags::None);
        }
    }

    LoggerSink(const LoggerSink&) = delete;
    LoggerSink& operator=(const LoggerSink&) = delete;

    [[nodiscard]] os_t& stream() noexcept { return *out_; }

    template<traits::Ostreamable... Args>
    os_t& write(const Args&... args) noexcept {
        sync_output();
        if (out_) {
            try {
                traits::write_sequence(*out_, args...);
                restore_output();
                flush();
                return *out_;
            } catch (...) {}
        }
        cfg_.set_logger_flag(Flags::LoggerWriteFailed);
        restore_output();
        return std::cerr;
    }

    void flush() noexcept {
        if (out_) {
            try { out_->flush(); } catch (...) {}
        }
    }

    [[nodiscard]] bool is_valid() const noexcept { return out_ != nullptr; }

    [[nodiscard]] bool is_buffered() const noexcept {
        using namespace rt;
        return (mode_ & OutputChannelMask) == Output::Buffer;
    }

private:
    rt::Output mode_ = rt::Output::Stdout;
    sv_t       raw_path_{};
    LogConfig& cfg_  = log_config;
    os_t*      out_  = nullptr;
    ofs_t      file_;
    bool       fatal_on_failure_ = false;
    rt::Output cached_output_ = rt::Output::Stdout;

    rt::Return init() {
        using namespace rt;
        if ((mode_ & OutputChannelMask) == Output::Stdout) {
            out_ = &std::cout;
            return ok();
        }
        if ((mode_ & OutputChannelMask) == Output::Stderr) {
            out_ = &std::cerr;
            return ok();
        }
        if ((mode_ & OutputChannelMask) == Output::Buffer) {
            try {
                out_ = &get_buffer<LoggerSink>();
                return ok();
            } catch (...) { return error("failed to acquire logging buffer"); }
        }
        if ((mode_ & OutputChannelMask) == Output::File) {
            Return status = open_output_file(file_, raw_path_, mode_);
            if (status.ok()) out_ = &file_;
            return status;
        }
        return error("unrecognized or unsupported output mode");
    }

    void sync_output() noexcept {
        cached_output_ = cfg_.output;
        cfg_.output = mode_;
    }

    void restore_output() noexcept {
        cfg_.output = cached_output_;
    }
};

} // namespace logging


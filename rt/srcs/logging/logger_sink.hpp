#pragma once
#include "../config.hpp"
#include <string_view>
#include <ostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include "traits.hpp"
#include "logging_utils.hpp"

namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;
using oss_t = std::ostringstream;
using ofs_t = std::ofstream;

using Flags             = rt::LoggerStatusFlags;
using LogConfig         = rt::Config;
inline auto& log_config = rt::config;

class LoggerSink {
public:
    explicit LoggerSink(rt::Output mode, sv_t raw_path = {},
                                         bool fatal_on_failure = false) noexcept
    { init(mode, raw_path, fatal_on_failure); }

    ~LoggerSink() noexcept {
        using namespace rt;
        flush();
        close_file();
        if (cfg_.warns_allowed && cfg_.logger_flags != 0) {
            write_logger_warns(out_ ? *out_ : std::cerr, cfg_.logger_flags);
            cfg_.logger_flags = static_cast<flags_t>(Flags::None);
        }
    }

    LoggerSink(const LoggerSink&) = delete;
    LoggerSink& operator=(const LoggerSink&) = delete;
    
    LoggerSink& init(rt::Output mode, sv_t raw_path = {},
                                         bool fatal_on_failure = false) noexcept
    {
        mode_ = mode;
        raw_path_ = raw_path;
        fatal_on_failure_ = fatal_on_failure;
        using namespace rt;
        if (auto status = init(); !status.ok()) {
            status.write(std::cerr, "LoggerSink");
            if (fatal_on_failure_) {
                write_logger_warns(std::cerr, cfg_.logger_flags);
                fatal_exit(ExitCode::OutputFailure);
            }
        }
        return *this;
    }
    
    LoggerSink& flush() noexcept {
        if (is_valid()) {
            try { out_->flush(); } catch (...) {}
        }
        return *this;
    }

    LoggerSink& clear_buffer() noexcept {
        if (is_buffered()) {
            if (auto* oss = static_cast<oss_t*>(out_)) [[likely]] {
                oss->str({});
                oss->clear();
            }
        }
        return *this;
    }

    LoggerSink& close_file() noexcept {
        if (file_is_open()) {
            try {
                file_.close();
            } catch (...) {
                cfg_.set_logger_flag(Flags::LoggerFileCloseFailed);
            }
        }
        return *this;
    }

    template<traits::Ostreamable... Args>
    LoggerSink& print(const Args&... args) noexcept {
        using namespace rt;
        if (!out_) return *this;
        ScopedOverride<Output> scope(cfg_.output, mode_);
        try {
            traits::write_sequence(*out_, args...);
            flush();
            return *this;
        } catch (...) {
            out_ = nullptr;
            cfg_.set_logger_flag(Flags::LoggerWriteFailed);
            return *this;
        }
    }

    [[nodiscard]] sv_t view_buffer() const noexcept {
        if (is_buffered())
            if (auto* oss = static_cast<oss_t*>(out_)) [[likely]]
                return oss->view();
        return {};
    }
    
    [[nodiscard]] bool is_valid() const noexcept { return out_ != nullptr; }

    [[nodiscard]] bool is_buffered() const noexcept {
        using namespace rt;
        return (mode_ & OutputChannelMask) == Output::Buffer;
    }

    [[nodiscard]] bool buffer_is_empty() const noexcept {
        return view_buffer().empty();
    }

    [[nodiscard]] bool file_is_open() const noexcept {
        using namespace rt;
        return (mode_ & OutputChannelMask) == Output::File && file_.is_open();
    }

private:
    rt::Output mode_ = rt::Output::Stdout;
    sv_t       raw_path_{};
    LogConfig& cfg_  = log_config;
    os_t*      out_  = nullptr;
    ofs_t      file_;
    bool       fatal_on_failure_ = false;

    rt::Return init() {
        using namespace rt;
        flush();
        close_file();
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
                out_ = &get_buffer<LoggerSink>(true);
                return ok();
            } catch (...) { return error("Failed to acquire logging buffer"); }
        }
        if ((mode_ & OutputChannelMask) == Output::File) {
            Return status = open_output_file(file_, raw_path_, mode_);
            if (status.ok()) out_ = &file_;
            return status;
        }
        return error("Unrecognized or unsupported output mode");
    }
};

} // namespace logging


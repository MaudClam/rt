#pragma once
#include "../config.hpp"
#include <string_view>
#include <ostream>
#include <sstream>
#include <fstream>
#include <functional>

namespace logging {

using sv_t   = std::string_view;
using os_t   = std::ostream;
using oss_t  = std::ostringstream;
using ofs_t  = std::ofstream;
using lock_t = std::lock_guard<std::mutex>;

struct LoggerSink {
    friend struct LoggerBase;

    explicit LoggerSink(io::Output mode, sv_t raw_path = {},
                                       bool fatal_on_failure = false) noexcept {
        init(mode, raw_path, fatal_on_failure);
    }

    ~LoggerSink() noexcept {
        using namespace rt;
        cfg().flush_log_warns();
        close_file();
    }

    LoggerSink(const LoggerSink&) = delete;
    LoggerSink& operator=(const LoggerSink&) = delete;
    
    LoggerSink& init_default() noexcept {
        return init(cfg().log_out, cfg().log_file.view(), true);
    }

    LoggerSink& init(io::Output mode, sv_t raw_path = {},
                                       bool fatal_on_failure = false) noexcept {
        using namespace common;
        close_file();
        mode_ = mode;
        raw_path_ = raw_path;
        fatal_on_failure_ = fatal_on_failure;
        if (const auto ret = setup_output_stream(); !ret.ok()) {
            report_error(ret, "LoggerSink init error:");
            if (fatal_on_failure_) {
                cfg().flush_log_warns();
                fatal_exit(ExitCode::LoggingFailure);
            }
        }
        return *this;
    }
    
    LoggerSink& clear_buffer() noexcept {
        if (is_buffered()) {
            buff_.str({});
            buff_.clear();
            buff_.flags(std::ios_base::fmtflags{});
            buff_.precision(6);
        }
        return *this;
    }

    LoggerSink& close_file() noexcept {
        if (file_is_open()) {
            try { file_.close(); }
            catch (...) { cfg().log_warns.set(Warn::LoggerFileCloseFailed); }
            if (out_ == &file_) out_ = nullptr;
        }
        return *this;
    }

    [[nodiscard]] sv_t view_buffer() const noexcept {
        if (is_buffered())
            return buff_.view();
        return {};
    }

    [[nodiscard]] bool is_buffered() const noexcept {
        return (mode_ & io::ChannelMask) == io::Output::Buffer;
    }

    [[nodiscard]] bool buffer_is_empty() const noexcept {
        return view_buffer().empty();
    }

    [[nodiscard]] bool file_is_open() const noexcept {
        return (mode_ & io::ChannelMask) == io::Output::File &&
                file_.is_open();
    }

    [[nodiscard]] bool good() const noexcept {
        return out_ && out().good();
    }

    [[nodiscard]] io::Output get_mode() const noexcept { return mode_; }

private:
    io::Output  mode_ = io::Output::Stdout;
    sv_t        raw_path_{};
    rt::Config* cfg_ = &rt::config;
    std::mutex  owned_mtx_;
    std::mutex* mtx_{&owned_mtx_};
    os_t*       out_ = nullptr;
    ofs_t       file_;
    oss_t       buff_;
    bool        fatal_on_failure_ = false;
    
    [[nodiscard]] const rt::Config& cfg() const noexcept {
        assert(cfg_ && "Internal error: cfg_ should never be null");
        return *cfg_;
    }

    [[nodiscard]] rt::Config& cfg() noexcept {
        assert(cfg_ && "Internal error: cfg_ should never be null");
        return *cfg_;
    }
    
    [[nodiscard]] std::mutex& mtx() const noexcept {
        assert(mtx_ && "Internal error: mtx_ should never be null");
        return *mtx_;
    }
    
    [[nodiscard]] os_t& out() const noexcept {
        assert(out_ && "Internal error: out_ should never be null");
        return *out_;
    }
    
    common::Return setup_output_stream() noexcept {
        using namespace common;
        if ((mode_ & io::ChannelMask) == io::Output::Stdout) {
            mtx_ = &stdout_mutex;
            out_ = &std::cout;
            return ok();
        }
        if ((mode_ & io::ChannelMask) == io::Output::Stderr) {
            mtx_ = &stderr_mutex;
            out_ = &std::cerr;
            return ok();
        }
        if ((mode_ & io::ChannelMask) == io::Output::Buffer) {
            out_ = &buff_;
            out().exceptions(std::ios::badbit | std::ios::failbit);
            return ok();
        }
        if ((mode_ & io::ChannelMask) == io::Output::File) {
            const Return st = open_output_file(file_, raw_path_, mode_);
            if (st.ok()) {
                out_ = &file_;
                file_.exceptions(std::ios::badbit | std::ios::failbit);
            }
            return st;
        }
        return error("Unrecognized or unsupported output mode");
    }

    void on_write_failure(bool failed) noexcept {
        if (!failed) return;
        cfg().log_warns.set(Warn::LoggerWriteFailed);
        out_ = nullptr;
        if (fatal_on_failure_) {
            cfg().flush_log_warns();
            fatal_exit(common::ExitCode::LoggingFailure);
        }
    }
};

} // namespace logging


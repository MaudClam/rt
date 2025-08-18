#pragma once
#include "../config.hpp"
#include <string_view>
#include <ostream>
#include <sstream>
#include <fstream>
#include "traits.hpp"
#include "format.hpp"

namespace logging {

using sv_t  = std::string_view;
using os_t  = std::ostream;
using oss_t = std::ostringstream;
using ofs_t = std::ofstream;

class LoggerSink {
public:
    explicit LoggerSink(io::Output mode, sv_t raw_path = {},
                                         bool fatal_on_failure = false) noexcept
    { init(mode, raw_path, fatal_on_failure); }

    ~LoggerSink() noexcept {
        using namespace rt;
        flush();
        cfg().flush_log_warns(std::cerr);
        close_file();
    }

    LoggerSink(const LoggerSink&) = delete;
    LoggerSink& operator=(const LoggerSink&) = delete;
    
    LoggerSink& init(io::Output mode, sv_t raw_path = {},
                                         bool fatal_on_failure = false) noexcept
    {
        using namespace rt;
        flush();
        close_file();
        mode_ = mode;
        raw_path_ = raw_path;
        fatal_on_failure_ = fatal_on_failure;
        if (auto status = setup_output_stream(); !status.ok()) {
            status.write_error("LoggerSink error:") ;
            if (fatal_on_failure_) {
                cfg().flush_log_warns(out());
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
                cfg().log_warns.set(Warn::LoggerFileCloseFailed);
            }
            if (out_ == &file_)
                out_ = nullptr;
        }
        return *this;
    }

    template<traits::Ostreamable... Args>
    LoggerSink& print(const Args&... args) noexcept {
        return try_write([&] {
            traits::write_sequence(*out_, args...);
        });
    }

    template<traits::Ostreamable... Args>
    LoggerSink& print(Format& fmt, const Args&... args) noexcept {
        return try_write([&] {
            fmt.apply(*out_, args...);
        });
    }

    [[nodiscard]] os_t& out() const noexcept {
        if (is_valid())
            if (auto* os = static_cast<os_t*>(out_)) [[likely]]
                return *os;
        return std::cerr;
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
        return (mode_ & io::OutputChannelMask) == io::Output::Buffer;
    }

    [[nodiscard]] bool buffer_is_empty() const noexcept {
        return view_buffer().empty();
    }

    [[nodiscard]] bool file_is_open() const noexcept {
        using namespace rt;
        return (mode_ & io::OutputChannelMask) == io::Output::File &&
                file_.is_open();
    }

private:
    io::Output  mode_ = io::Output::Stdout;
    sv_t        raw_path_{};
    rt::Config* cfg_ = &rt::config;
    os_t*       out_ = nullptr;
    ofs_t       file_;
    bool        fatal_on_failure_ = false;
    
    [[nodiscard]] const rt::Config& cfg() const noexcept {
        assert(cfg_ && "Internal error: cfg_ should never be null");
        return *cfg_;
    }

    [[nodiscard]] rt::Config& cfg() noexcept {
        assert(cfg_ && "Internal error: cfg_ should never be null");
        return *cfg_;
    }

    rt::Return setup_output_stream() {
        using namespace rt;
        if ((mode_ & io::OutputChannelMask) == io::Output::Stdout) {
            out_ = &std::cout;
            return ok();
        }
        if ((mode_ & io::OutputChannelMask) == io::Output::Stderr) {
            out_ = &std::cerr;
            return ok();
        }
        if ((mode_ & io::OutputChannelMask) == io::Output::Buffer) {
            try {
                out_ = &get_buffer<LoggerSink>(true);
                return ok();
            } catch (...) { return error("Failed to acquire logging buffer"); }
        }
        if ((mode_ & io::OutputChannelMask) == io::Output::File) {
            Return status = open_output_file(file_, raw_path_, mode_);
            if (status.ok()) out_ = &file_;
            return status;
        }
        return error("Unrecognized or unsupported output mode");
    }
    
    template<typename Writer>
    LoggerSink& try_write(Writer&& f) noexcept {
        using namespace rt;
        if (!out_) return *this;
        ScopedOverride<io::Output> scope(cfg().log_out, mode_);
        try {
            std::forward<Writer>(f)();
            flush();
        } catch (...) {
            out_ = nullptr;
            cfg().log_warns.set(Warn::LoggerWriteFailed);
        }
        return *this;
    }
};

} // namespace logging


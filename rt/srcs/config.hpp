#pragma once
#include <string>
#include <exception>
#include <filesystem>
#include <clocale>
#include <cstdlib>
#include <cstddef>
#include "common/common.hpp"
#include "logging/ansi_enums_naming.hpp"
#include "logging/logging_warns.hpp"
#include "logging/io_channels.hpp"
#if defined(_WIN32)
  #include <io.h>
#else
  #include <unistd.h>
#endif


namespace rt {

using common::sv_t;
using common::oss_t;
namespace fs = std::filesystem;

struct Config;
extern Config config;
inline constexpr sv_t default_config_file = "config.ini";
inline constexpr sv_t default_log_file    = "rt.log";

inline constexpr sv_t help = R"h(For help: program --help
)h";

inline constexpr sv_t cmdline_help = R"help(Usage: program [flags] [parameters]
Flags:
  --help              This help
  --no-tty            Disable TTY detection
  --no-ansi           Disable ANSI escape codes
  --no-utf8           Disable UTF-8 output, fallback to ASCII
  --no-emoji          Disable emoji in output
  --no-warns          Disable deferred logger warnings
  --config-dump       Write config dump - !defined(NDEBUG) only
  --metrics-dump      Write metrics dump - !defined(NDEBUG) only
Parameters:
  --config=path       Path to the another user defined config file
  --log-out=target    Logging output (stdout, stderr, file)
  --log-file=path     Path to log file
  --test-string=value Test string value
  --test-param=N      Test numeric parameter

)help";

struct Config {
    using Buff512  = common::RawBuffer<512>;
    using Buff32   = common::RawBuffer<32>;
    using Output   = logging::io::Output;
    using LogWarns = logging::LogWarns;
    using Warn     = logging::Warn;
    using flags_t  = logging::flags_t;

	// Logging
    ansi::Color      tty_foreground = ansi::Color::White;
    ansi::Background tty_background = ansi::Background::Black;
	bool    tty_allowed   = true;
	bool    ansi_allowed  = true;
	bool    utf8_inited   = true;
    bool    emoji_allowed = true;
    bool    warns_allowed = true;
    Output  log_out       = Output::Stderr;
    Buff32  log_file      = default_log_file;
    mutable LogWarns log_warns;

	// Common
    Buff512 config_file  = default_config_file;
    bool    config_dump  = false;
    bool    metrics_dump = false;
    int     test_param   = 0;
    Buff512 test_string{};
    // ...

    ~Config() noexcept {
        if constexpr (debug_mode) {
            using namespace common;
            if (config_dump) flush_config_dump();
            if (metrics_dump || is_metrics()) flush_metrics_dump();
        }
        this->flush_log_warns();
    }

    void init(int ac, char** av) noexcept {
        using namespace common;
        load_config_file(ac, av);
        Return status = parse_cmdline(ac, av);
        handle_error({}, status.fatal_on(), ExitCode::CmdlineFailure, help);
        detect_environment();
    }

    void detect_environment() noexcept {
        tty_allowed  = tty_allowed && detect_is_terminal();
        ansi_allowed = ansi_allowed && tty_allowed;
        if (utf8_inited) {
            if (environment_declares_utf8()) {
                utf8_inited = try_activate_utf8_locale();
                if (!utf8_inited)
                    log_warns.set(Warn::LocaleActivationFailed);
            } else {
                utf8_inited = false;
            }
        }
    }

    void flush_log_warns(bool test = false) noexcept {
        using namespace common;
        if ((!warns_allowed || !log_warns.any()) && !test) return;
        auto& table = logging::log_warn_descriptions;
        if (test) log_warns.set_all_descriptive_warns(table);
        RawBuffer<1024> buf("\n");
        for (const auto& entry : table)
            if (log_warns.test(entry.value))
                buf.append("[LOG_WARN] ", entry.message, '\n');
        if (test) {
            buf.add_prefix({"\n"}, "TEST LOG WARNS =============\n");
            buf.append("TEST LOG WARNS END =========\n\n");
        }
        buf.finalize_ellipsis_newline(true);
        raw_write_stderr(buf);
        log_warns.reset();
    }

#if !defined(NDEBUG)
    void flush_config_dump() const noexcept {
        using namespace common;
        RawBuffer<4096>
        buf(
            "\nCONFIG DUMP ================\n",
            "Logging:\n",
            "  tty_foreground: ", tty_foreground, '\n',
            "  tty_background: ", tty_background, '\n',
            "  tty_allowed:    ", tty_allowed,    '\n',
            "  ansi_allowed:   ", ansi_allowed,   '\n',
            "  utf8_inited:    ", utf8_inited,    '\n',
            "  emoji_allowed:  ", emoji_allowed,  '\n',
            "  warns_allowed:  ", warns_allowed,  '\n',
            "  log_out:        ", log_out,        '\n',
            "  log_file:       '",log_file,      "'\n",
            "  log_warns:      ", std::bitset<8>(log_warns.bits()),'\n',
            "Common:\n",
            "  config_file:    '",config_file,   "'\n",
            "  config_dump:    ", config_dump,    '\n',
            "  metrics_dump:   ", metrics_dump,   '\n',
            "  test_param:     ", test_param,     '\n',
            "  test_string:    '",test_string,   "'\n",
            "CONFIG DUMP END ============\n"
            );
        buf.finalize_ellipsis_newline(true);
        raw_write_stderr(buf);
    }
#endif //NDEBUG

private:
    void load_config_file(int ac, char** av) noexcept {
        using namespace common;
        using namespace logging;
        fs::path  path{};
        io::ifs_t file{};
        handle_error("Error config file path:",
                     config_path(path, ac, av).fatal_on(),
                     ExitCode::CfgFileFailure);
        handle_error("Error caching config file path:",
                     cach_config_file_path(path).fatal_off(),
                     ExitCode::CfgFileFailure);
        handle_error("Error open config file:",
                     io::open_input_file(file, path).fatal_on(),
                     ExitCode::CfgFileFailure);
        handle_error("┌─ Error parsing config file:",
                     parse_config_file(file, tl_copy(path)).fatal_on(),
                     ExitCode::CfgFileFailure);
    }

    [[nodiscard]] common::Return
    config_path(fs::path& path, int ac, char** av) noexcept {
        using namespace common;
        using namespace logging::io;
        sv_t key = "--config=";
        sv_t path_sv{config_file.view()};
        for (int i = 1; i < ac; ++i) {
            sv_t sv_arg{av[i]};
            if (sv_arg.starts_with(key))
                path_sv = trim(safe_substr(sv_arg, key.size()));
        }
        auto p = check_possible_paths(path_sv, {"..", "../.."});
        if (!p) return error("file not found", path_sv, true);
        path = *p;
        return ok();
}

    [[nodiscard]] common::Return
    cach_config_file_path(const fs::path& path) noexcept {
        if (auto status = config_file.load(path.c_str()); !status.ok()) {
            config_file = logging::tl_copy(path, config_file.capacity());
            return status.set_prompt(config_file.view());
        }
        return common::ok();
    }

    [[nodiscard]] common::Return
    parse_config_file(logging::io::ifs_t& file, sv_t prompt) noexcept {
        using namespace common;
        using namespace logging::io;
        Return status;
        auto&  buf = tl_buffer();
        int    n = 0, err = 0;
        bool   fatal = false;
        try {
            std::string line{};
            while (std::getline(file, line)) {
                ++n;
                if (line.empty() || line.starts_with('#')) continue;
                status = apply_arg(line);
                if (!status.ok()) {
                    err++;
                    if (err == 1) buf << '\'' << prompt << "'\n";
                    buf << "│ line:" << n << ": " << status << '\n';
                    fatal |= status.fatal;
                }
            }
            if (err > 0) buf << "└─ [end]";
        } catch (const std::exception& e) {
            return error("Failed to parse config file", e.what(), true);
        } catch (...) {
            return error("Unknown error while parsing config file.", {}, true);
        }
        if (!buf.view().empty()) return error(buf.view(), {}, fatal );
        return ok();
    }

    [[nodiscard]] common::Return parse_cmdline(int ac, char** av) noexcept {
        using namespace common;
        for (int i = 1; i < ac; ++i) {
            if (!av[i]) return error("Null argument", "argv[i]", true);
            sv_t sv_arg{av[i]};
            if (sv_arg == "--help" || sv_arg == "-h" || sv_arg == "/h") {
                (void)with_stdout(cmdline_help);
                graceful_exit();
            }
            if (!sv_arg.starts_with("--"))
                return error("Unrecognized argument", sv_arg, true);
            if (auto status = apply_arg(safe_substr(sv_arg, 2)); !status.ok())
                return status.set_prompt(sv_arg);
        }
        return ok();
    }

    [[nodiscard]] common::Return apply_arg(sv_t sv_arg) noexcept {
        if (sv_arg.find('=') != sv_t::npos)
            return apply_param(sv_arg);
        return apply_flag(sv_arg);
    }

    [[nodiscard]] common::Return apply_flag(sv_t sv_arg) noexcept {
        using namespace common;
        switch (hash_31(sv_arg)) {
            case hash_31("no-tty"):       tty_allowed   = false; break;
            case hash_31("no-ansi"):      ansi_allowed  = false; break;
            case hash_31("no-utf8"):      utf8_inited   = false; break;
            case hash_31("no-emoji"):     emoji_allowed = false; break;
            case hash_31("no-warns"):     warns_allowed = false; break;
            case hash_31("config-dump"):  config_dump   = true;  break;
            case hash_31("metrics-dump"): metrics_dump  = true;  break;
            default: return error("Unrecognized flag", sv_arg, true);
        }
        return ok();
    }

    [[nodiscard]] common::Return apply_param(sv_t arg) noexcept {
        using namespace common;
        size_t pos = arg.find('=');
        if (pos == sv_t::npos)
            return error("Expected '=' in parameter", arg, true);
        sv_t key = trim(safe_substr(arg, 0, pos));
        sv_t val = trim(safe_substr(arg, pos + 1));
        switch (hash_31(key)) {
            case hash_31("tty-foreground"): {
                ansi::Color c = find_named_enum(ansi::enumColors, val);
                if (c == static_cast<ansi::Color>(-1))
                    return error("Unknown tty foreground color", val, true);
                tty_foreground = c;
                break;
            }
            case hash_31("tty-background"): {
                ansi::Background b = find_named_enum(ansi::enumBackgrounds,val);
                if (b == static_cast<ansi::Background>(-1))
                    return error("Unknown tty background color", val, true);
                tty_background = b;
                break;
            }
            case hash_31("log-out"): {
                if      (val == "stdout") log_out = Output::Stdout;
                else if (val == "stderr") log_out = Output::Stderr;
                else if (val == "file")   log_out = Output::File |
                                                    Output::CreateDirs |
                                                    Output::Indexing |
                                                    Output::TimeIndex;
                else return error("Invalid value for log-out", val, true);
                break;
            }
            case hash_31("log-file"):    return log_file.set(val,key).fatal_on();
            case hash_31("test-string"): return test_string.set(val,key).fatal_on();
            case hash_31("test-param"):  test_param = parse_int(val); break;
            case hash_31("config"):      break;// parsed in config_path()
            default: return error("Unrecognized parameter", key, true);
        }
        return ok();
    }

    [[nodiscard]] static bool detect_is_terminal() noexcept {
        #if defined(_WIN32)
                return false;
        #else
                return isatty(fileno(stdout));
        #endif
    }

    [[nodiscard]] static bool contains_utf8_ci(sv_t v) noexcept {
        const auto tolow = [](unsigned char c) noexcept -> char {
            if (c >= 'A' && c <= 'Z') return char(c - 'A' + 'a');
            return char(c);
        };
        const size_t n = v.size();
        for (size_t i = 0; i < n; ++i) {
            if (tolow(v[i]) == 'u' && i + 3 < n &&
                tolow(v[i + 1]) == 't' && tolow(v[i + 2]) == 'f')
            {
                size_t j = i + 3;
                if (j < n && (v[j] == '-' || v[j] == '_')) ++j;
                if (j < n && v[j] == '8') return true;
            }
        }
        return false;
    }

    [[nodiscard]] static bool environment_declares_utf8() noexcept {
        if (const char* lc_all = std::getenv("LC_ALL")) {
            return contains_utf8_ci(sv_t{lc_all});
        }
        if (const char* lc_ctype = std::getenv("LC_CTYPE")) {
            return contains_utf8_ci(sv_t{lc_ctype});
        }
        if (const char* lang = std::getenv("LANG")) {
            return contains_utf8_ci(sv_t{lang});
        }
        return false;
    }

    [[nodiscard]] static bool try_activate_utf8_locale() noexcept {
        if (const char* r = std::setlocale(LC_CTYPE, "")) {
            return contains_utf8_ci(sv_t{r});
        }
        return false;
    }
};

} // namespace rt

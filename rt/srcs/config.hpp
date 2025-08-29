#pragma once
#include <string_view>
#include <ostream>
#include <string>
#include <unistd.h>
#include "common/common.hpp"
#include "logging/ansi_enums_naming.hpp"
#include "logging/logging_warns.hpp"
#include "logging/io_channels.hpp"

namespace rt {

using sv_t   = std::string_view;
using oss_t  = std::ostringstream;
namespace fs = std::filesystem;

struct Config;
extern Config config;
inline constexpr sv_t default_config_file = "руссзавайся";
inline constexpr sv_t default_log_file    = "rt.log";

fs::path get_exec_path();

inline constexpr sv_t cmdline_help = R"help(
Usage: program [flags] [parameters]

Flags:
  --no-tty      Disable TTY detection
  --no-ansi     Disable ANSI escape codes
  --no-utf8     Disable UTF-8 output, fallback to ASCII
  --no-emoji    Disable emoji in output
  --no-warns    Disable deferred logger warnings
  --config-dump Write config dump

Parameters:
  --config=path         Path to the another user defined config file
  --log-out=target      Logging output (stdout, stderr, file)
  --log-file=path       Path to log file
  --test-string=value   Test string value
  --test-param=N        Test numeric parameter

)help";

struct Config {
    using Buffer512 = common::StrBuffer<512>;
    using Buffer32  = common::StrBuffer<32>;
    using Output    = logging::io::Output;
    using LogWarns  = logging::LogWarns;
    using Warn      = logging::Warn;
    using flags_t   = logging::flags_t;

	// Logging
    ansi::Color      tty_foreground = ansi::Color::White;
    ansi::Background tty_background = ansi::Background::Black;
	bool tty_allowed    = true;
	bool ansi_allowed   = true;
	bool utf8_inited    = true;
    bool emoji_allowed  = true;
    bool warns_allowed  = true;
    Output    log_out   = Output::Stderr;
    Buffer512 log_file  = default_log_file;
    mutable LogWarns log_warns;

	// Common
    Buffer32  config_file = default_config_file;
    bool      config_dump = false;
    Buffer512 test_string;
    int       test_param = 0;
    // ...

    ~Config() noexcept {
        if constexpr (debug_mode)
            if (config_dump)
                flush_config_dump();
        flush_log_warns();
    }
    
    void flush_log_warns() noexcept {
        if (warns_allowed && !log_warns.test(Warn::None))
            common::with_stderr(log_warns);
    }

    void flush_config_dump() const noexcept {
        using namespace common;
        with_stderr(
                    "\nCONFIG DUMP ================\n", std::boolalpha,
                    "Logging:\n",
                    "  tty_foreground: ", tty_foreground,  '\n',
                    "  tty_background: ", tty_background,  '\n',
                    "  tty_allowed:    ", tty_allowed,     '\n',
                    "  ansi_allowed:   ", ansi_allowed,    '\n',
                    "  utf8_inited:    ", utf8_inited,     '\n',
                    "  emoji_allowed:  ", emoji_allowed,   '\n',
                    "  warns_allowed:  ", warns_allowed,   '\n',
                    "  log_out:        ", log_out,         '\n',
                    "  log_file:       '",log_file.view(),"'\n",
                    "  log_warns:      ", std::bitset<8>(log_warns.bits()), '\n',
                    "Common:\n",
                    "  config_file:    ", config_file.view(),  '\n',
                    "  config_dump:    ", config_dump,         '\n',
                    "  test_string:    '",test_string.view(), "'\n",
                    "  test_param:     ", test_param,          '\n',
                    "CONFIG DUMP END ============\n\n"
                    );
    }

    Config& init(int ac, char** av) noexcept {
        using namespace common;
        detect_environment();
        try {
            load_config_file(config_path(ac, av));
        } catch (const std::exception& e) {
            with_stderr("Failed to load config file: ", e.what(), '\n');
        } catch (...) {
            with_stderr("Unknown error while loading config file.\n");
        }
        parse_cmdline(ac, av);
        if constexpr (debug_mode)
            if (config_dump)
                flush_config_dump();
        return *this;
    }

private:
    Config& load_config_file(const fs::path& path) {
        using namespace common;
        using namespace logging::io;
        ifs_t  file;
        Return st = open_input_file(file, path);
        if (!st.ok()) {
            print_error("Error open config file:", st);
            return *this;
        }
        bool  fatal = false;
        int   num = 0;
        oss_t errors;
        std::string line;
        while (std::getline(file, line)) {
            ++num;
            if (line.empty() || line.starts_with('#')) continue;
            st = apply_arg(line);
            if (!st.ok()) {
                errors << logging::tl_copy(path) << ":" << num << ": ";
                errors << "[ERROR]: " << st << '\n';
                fatal |= st.fatal_err;
            }
        }
        if (!errors.view().empty()) {
            with_stderr(errors.view());
            if (fatal) fatal_exit(ExitCode::CfgFileFailure);
        }
        file.close();
        return *this;
    }

    Config& parse_cmdline(int ac, char** av) noexcept {
        using namespace common;
        Return st{ "Null argument", "argv[i]" };
		for (int i = 1; i < ac; ++i) {
            if (!av[i])
                fatal_exit(print_error({}, st, ExitCode::CmdlineFailure));
            sv_t sv_arg{av[i]};
            if (sv_arg == "--help" || sv_arg == "-h" || sv_arg == "/h") {
                with_stdout(cmdline_help);
                graceful_exit();
            }
            if (sv_arg.starts_with("--")) {
                st = apply_arg(safe_substr(sv_arg, 2));
                if (!st.ok()) st.prompt = sv_arg;
            } else {
                st = { "Unrecognized argument", sv_arg };
            }
            if (!st.ok())
                fatal_exit(print_error({}, st, ExitCode::CmdlineFailure));
		}
        return *this;
	}

    Config& detect_environment() noexcept {
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
        return *this;
    }

    [[nodiscard]] fs::path config_path(int ac, char** av) {
        using namespace common;
        using namespace logging;
        fs::path cfg_path{config_file.view()};
        sv_t     cfg_path_sv{};
        sv_t     key = "--config=";
        for (int i = 1; i < ac; ++i) {
            sv_t sv_arg{av[i]};
            if (sv_arg.starts_with(key))
                cfg_path_sv = trim(safe_substr(sv_arg, key.size()));
        }
        if (!cfg_path_sv.empty()) cfg_path = cfg_path_sv;
        std::error_code ec;
        if (!(cfg_path.is_absolute() && fs::exists(cfg_path, ec) && !ec)) {
            fs::path possible_paths[] = {
                cfg_path,
                get_exec_path().parent_path() / cfg_path,
                fs::current_path() / cfg_path,
                fs::path{".."} / cfg_path,
                fs::path{"../.."} / cfg_path,
            };
            for (const auto& raw : possible_paths) {
                fs::path p = fs::weakly_canonical(raw, ec);
                if constexpr (debug_mode)
                    with_stderr("Finding config file: ", tl_copy(p), '\n');
                if (fs::exists(p, ec)) {
                    cfg_path = p;
                    break;
                }
            }
        }
        cfg_path_sv = tl_copy(cfg_path);
//        cfg_path_sv = tl_copy(cfg_path, config_file.get_capasity());
        [[maybe_unused]] Return r = config_file.set(cfg_path_sv, key);
        return cfg_path;
    }

    [[nodiscard]] common::Return apply_arg(sv_t sv_arg) noexcept {
        if (sv_arg.find('=') != sv_t::npos)
            return apply_param(sv_arg);
        return apply_flag(sv_arg);
    }

    [[nodiscard]] common::Return apply_flag(sv_t sv_arg) noexcept {
        using namespace common;
        switch (hash_31(sv_arg)) {
            case hash_31("no-tty"):      tty_allowed   = false; break;
            case hash_31("no-ansi"):     ansi_allowed  = false; break;
            case hash_31("no-utf8"):     utf8_inited   = false; break;
            case hash_31("no-emoji"):    emoji_allowed = false; break;
            case hash_31("no-warns"):    warns_allowed = false; break;
            case hash_31("config-dump"): config_dump   = true;  break;
            default: return error("Unrecognized flag", sv_arg, true);
        }
        return ok();
    }

    [[nodiscard]] common::Return apply_param(sv_t arg) noexcept {
        using namespace common;
        size_t pos = arg.find('=');
        if (pos == sv_t::npos) return error("Expected '=' in parameter", arg);
        sv_t key = trim(safe_substr(arg, 0, pos));
        sv_t val = trim(safe_substr(arg, pos + 1));
        switch (hash_31(key)) {
            case hash_31("tty-foreground"): {
                ansi::Color c = find_named_enum(ansi::enumColors, val);
                if (c == static_cast<ansi::Color>(-1))
                    return error("Unknown tty foreground color", val);
                tty_foreground = c;
                break;
            }
            case hash_31("tty-background"): {
                ansi::Background b = find_named_enum(ansi::enumBackgrounds, val);
                if (b == static_cast<ansi::Background>(-1))
                    return error("Unknown tty background color", val);
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
                else return error("Invalid value for log-out", val);
                break;
            }
            case hash_31("log-file"):    return log_file.set(val, key);
            case hash_31("test-string"): return test_string.set(val, key);
            case hash_31("test-param"):  test_param = parse_int(val); break;
            case hash_31("config"):      break;
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

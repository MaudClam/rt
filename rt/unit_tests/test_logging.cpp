#include <iostream>
#include <sstream>
#include <random>
#include "../srcs/config.hpp"
#include "../srcs/logging/logging.hpp"

// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_logging.cpp -o logging && ./logging

using str_t = std::string;
using strv_t = std::string_view;
using oss_t = std::ostringstream;

struct Range { int from = 0, to = 0; str_t groupe; };
std::vector<Range> emoji_ranges = {
    {0x1F600, 0x1F64F, "faces"},
    {0x1F300, 0x1F5FF, "symbols"},
    {0x1F680, 0x1F6FF, "transport"},
    {0x1F900, 0x1F9FF, "extension"},
    {0x2600,  0x26FF, "weather and stuff"},
    {0x2700,  0x27BF, "miscellaneous"}
};

[[nodiscard]]
inline str_t utf8_from_codepoint(char32_t cp) {
    str_t out;
    if (cp <= 0x7F) {
        out += static_cast<char>(cp);
    } else if (cp <= 0x7FF) {
        out += static_cast<char>(0xC0 | ((cp >> 6) & 0x1F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp <= 0xFFFF) {
        out += static_cast<char>(0xE0 | ((cp >> 12) & 0x0F));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp <= 0x10FFFF) {
        out += static_cast<char>(0xF0 | ((cp >> 18) & 0x07));
        out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
    return out;
}

[[nodiscard]]
inline str_t hex_codepoints(const str_t& str) {
    oss_t oss;
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.data());
    size_t len = str.size();
    for (size_t i = 0; i < len;) {
        char32_t cp = 0;
        size_t extra = 0;
        if (bytes[i] <= 0x7F) {
            cp = bytes[i];
            extra = 0;
        } else if ((bytes[i] & 0xE0) == 0xC0) {
            cp = bytes[i] & 0x1F;
            extra = 1;
        } else if ((bytes[i] & 0xF0) == 0xE0) {
            cp = bytes[i] & 0x0F;
            extra = 2;
        } else if ((bytes[i] & 0xF8) == 0xF0) {
            cp = bytes[i] & 0x07;
            extra = 3;
        }
        for (size_t j = 1; j <= extra; ++j)
            cp = (cp << 6) | (bytes[i + j] & 0x3F);
        oss << "U+" << std::hex << std::uppercase << static_cast<uint32_t>(cp);
        i += 1 + extra;
    }
    return oss.str();
}

[[nodiscard]]
inline str_t random_emoji() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    // random range
    std::uniform_int_distribution<> dist_range(0, int(emoji_ranges.size()) - 1);
    Range r = emoji_ranges[dist_range(gen)];
    // random code in range
    std::uniform_int_distribution<> dist_cp(r.from, r.to);
    return utf8_from_codepoint(dist_cp(gen));
}

struct Vec3f {
    float e[3]{};
};
std::ostream& operator<<(std::ostream& os, const Vec3f& vec) {
    for (int i = 0; i < 3; ++i)
        os << vec.e[i] << (i < 2 ? "," : "");
    return os;
}

namespace rt { Config config; }

// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_logging_utils.cpp -o test_logging_utils && ./test_logging_utils
int main(int ac, char** av) {
    using namespace logging;
    if (auto r = rt::config.parse_cmdline(ac, av); r) return r.write_error_if();
    rt::config.debug_print();

    
    CellFormat fmt{};
    fmt.precision.digits = 22;
    fmt.align.width = 5;
    fmt.ansi_style.use_ansi = false;
//    Cell<Vec3f> c = { .fmt = fmt, .suffix = "\n" };
    Cell c1 = { .fmt = fmt, .value = log_config.test_param };
    Cell<str_t> c2 = { .fmt = fmt };
//    c2.value = "Папа у Васи..";
    c2.value = "\tL\n";

//    std::cout << c;
    std::cout << c1 << "\n";
    std::cout << c2;
//    ProgressBar bar{};
//    bar.write(std::cout, 7);
//    bar.write(std::cout, 8);
//    bar.write(std::cout, 9);
//    bar.write(std::cout, 10);
//    bar.write(std::cout, 11);

//    for (int i = 0; i < 10; ++i) {
//        str_t emoji = random_emoji();
//        std::cout << emoji << " " << hex_codepoints(emoji) << "\n";
//    }

	return EXIT_SUCCESS;
}


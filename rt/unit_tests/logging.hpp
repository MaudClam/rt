#pragma once
#include <wchar.h>
#include <locale.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <map>

namespace testlog {

struct Format;
class  Logger;
class  Formatter;
inline int utf8_terminal_width(const std::string& utf8str);

extern Format format;


struct Format {
	int test_name_width        = 32;
	int indent_spaces          = 4;
	int displays_of_progress   = 10;
	bool bold_headers          = true;
};


// === Formatter class ===
class Formatter {
public:
	Formatter(int width = 0, int precision = -1)
		: width_(width), precision_(precision) {}
	
	template<typename T>
	std::string format(const T& value) const {
		std::ostringstream oss;
		oss << std::left;
		if (width_ > 0)
			oss << std::setw(width_);
		if constexpr (std::is_floating_point_v<T>) {
			if (precision_ >= 0)
				oss << std::fixed << std::setprecision(precision_);
		}
		oss << value;
		return oss.str();
	}

	void set_width(int w) { width_ = w; }
	void set_precision(int p) { precision_ = p; }

private:
	int width_, precision_;
	std::string prefix, suffix;
};


class Logger {
public:
	enum class Color { Default, Red, Green, Yellow, Cyan, Gray };
	
	Logger():
	use_color_(detect_color_support()),
	is_tty_(is_terminal())
	{}


private:
	bool use_color_;
	bool is_tty_;

	std::string get_indent(int indent = format.indent_spaces) {
		return std::string(indent, ' ');
	}
	std::string get_colored(const std::string& msg, Color color, bool bold = false) const {
		if (use_color_)
			return get_color_code(color, bold) + get_color_code(Color::Default, false);
		return msg;
	}
	std::string get_color_code(Color color, bool bold) const {
		const char* base = bold ? "1;" : "";
		switch (color) {
			case Color::Default: return "\033[0m";
			case Color::Red:     return std::string("\033[") + base + "31m";
			case Color::Green:   return std::string("\033[") + base + "32m";
			case Color::Yellow:  return std::string("\033[") + base + "33m";
			case Color::Cyan:    return std::string("\033[") + base + "36m";
			case Color::Gray:    return std::string("\033[") + base + "90m";
			default:             return "\033[0m";
		}
	}
	bool is_terminal() const {
#if defined(_WIN32)
		return false;
#else
		return isatty(fileno(stdout));
#endif
	}
	bool detect_color_support() const {
#if defined(_WIN32)
		return false;
#else
		return is_terminal();
#endif
	}

};


} // namespace testlog

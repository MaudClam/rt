#pragma once
#include <iostream>
#include <string>
#include <unistd.h> // isatty
#include <cstdlib>	// std::atoi
#include <locale>   // std::setlocale

namespace rt {

struct Config {
	// logging
	bool tty_allowed        = true;
	bool ansi_allowed       = true;
	bool utf8_inited		= true;
	bool emoji_allowed		= true;
	bool mirror_stdout      = false;
	bool interactive_mode   = false;
	bool log_to_file        = false;
	std::string log_file	= "rt.log";
	int indent_spaces		= 4;
	int test_param			= 3;
	// logging errors
	std::string logging_errors;

	// common parameters

	Config() {
		tty_allowed   = detect_is_terminal();
		ansi_allowed  = tty_allowed;
		utf8_inited   = ensure_utf8_locale();
		emoji_allowed = utf8_inited;
	}
	
	~Config() {
		if (logging_errors.size())
			std::cerr << logging_errors;
	}

	bool detect_is_terminal() {
	#if defined(_WIN32)
		return false;
	#else
		return isatty(fileno(stdout));
	#endif
	}
	
	bool ensure_utf8_locale() {
		return std::setlocale(LC_ALL, "") != nullptr;
	}
	
	void parse_args(int ac, char** av) {
		for (int i = 1; i < ac; ++i) {
			std::string arg = av[i];
			if (arg == "--no-tty") {
				tty_allowed = false;
			} else if (arg == "--no-ansi") {
				ansi_allowed = false;
			} else if (arg == "--no-utf8") {
				utf8_inited = false;
			} else if (arg == "--no-emoji") {
				emoji_allowed = false;
			} else if (arg == "--emoji") {
				emoji_allowed = true;
			} else if (arg == "--mirror_stdout") {
				mirror_stdout = true;
			} else if (arg == "--interactive") {
				interactive_mode = true;
			} else if (arg.rfind("--log_to_file=", 0) == 0) {
				log_to_file = true;
				std::string tmp = arg.substr(14);
				log_file = tmp.size() > 0 ? tmp : log_file;
			} else if (arg.rfind("--indent=", 0) == 0) {
				indent_spaces = std::atoi(arg.substr(9).c_str());
			} else if (arg.rfind("--test-param=", 0) == 0) {
				test_param = std::atoi(arg.substr(13).c_str());
			} else {
				std::cerr << "Unknown flag " << arg << std::endl;
				exit(-1);
			}
		}
	}
};

extern Config config;


} // namespace rt


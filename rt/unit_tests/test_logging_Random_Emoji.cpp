#include <iostream>
#include <string>
#include "../srcs/config.hpp"
#include "../srcs/logging/logging_utils.hpp"
#include "../srcs/logging/timing.hpp"

// g++ -std=c++2a -O2 test_logging_Random_Emoji.cpp -o RandomEmoji && ./RandomEmoji
// --test-param=100 to assign quantity
// --no-ansi to override ANSI escape codes

namespace rt { Config config; }

int main(int ac, char** av) {
	rt::config.parse_args(ac, av);
	using namespace logging;
//	const bool is_ansi = rt::config.ansi_allowed;
//	TimeCounter	t_counter;
//	int num = log_config.test_param;
//	int duration_ms = static_cast<int>(1e+4 / num);
//	str_t num_str;
//	int num_width = detail::estimate_terminal_width(num, num_str);
//	
//    // Preparing the output streams
//    std::ostringstream ansi_out;
//    auto cout = extend(std::cout);
//    auto out = extend(is_ansi ? ansi_out : std::cout);
//
//	// Headline
//	if (is_ansi)
//        cout << ansi::clear_screen << std::flush;
//	else
//        system("clear");
//    cout.with_align(num_str);
//    cout.with_align("Random Emoji:");
//    if (!is_ansi) cout << "\n";
//
//	// Main loop
//    for (int i = 0; i < num; ++i) {
//        str_t random_emoji;
//        const int emoji_len = detail::estimate_terminal_width(get_random_emoji(), random_emoji);
////        if (emoji_len == 0) { --i; continue; } // skip unsupported emoji
//
//        // Clear the ansi_out buffer if used
//        if (is_ansi) ansi_out.str("");
//        
//        // Form a string
//        out.with_align(i + 1, {.width=num_width,.alignment=Align::Right} );
//        out.with_align(random_emoji, {.width=4, .alignment=Align::Centred} );
//        out.with_align(get_hex_codepoints(random_emoji), {.width=8} );
//		out.with_align(emoji_len);
//
//		if (is_ansi) {
//			cout << ansi_out.str() << std::flush;
//			t_counter.run_ms(duration_ms);
//            cout << (i == num - 1 ? "\n"
//                     : ansi::clear_left(detail::utf8_terminal_width(ansi_out.str())));
//		} else
//			cout << "\n";
//	}
    
    with_label(std::cout, Label::Error, "Clear the ansi_out buffer if used.\n");
    with_label(std::cout, Label::Warning, "Clear the ansi_out buffer if used.\n");
    with_label(std::cout, Label::Info, "Clear the ansi_out buffer if used.\n");
    with_label(std::cout, Label::Debug, "Clear the ansi_out buffer if used.\n");
    with_label(std::cout, Label::Success, "Clear the ansi_out buffer if used.\n");
	return 0;
}

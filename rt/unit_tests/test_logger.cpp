#include "logging.hpp"
#include "TimeCounter.hpp"

// g++ -std=c++2a -O2 -Wall -Wextra -Werror test_logger.cpp -o test_logger && ./test_logger


int main() {
	using namespace testlog;
	format.test_name_width = 40;
	format.indent_spaces   = 2;
	format.bold_headers    = true;
	Formatter f1(10, 3);
	Formatter f2(10, 2);
	
	system("clear");
	std::cout << f1.format(123) << "|"
			  << f1.format("abc") << "|"
			  << f1.format(3.14159) << "\n";
	std::cout << f2.format(3.14159) << "\n";
	return 0;
}


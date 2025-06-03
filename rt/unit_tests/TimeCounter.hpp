#pragma once
#include <iostream>
#include <chrono>
#include <thread>

namespace testlog {

class TimeCounter {
public:
	explicit TimeCounter() : time_(1), sec_(true) {}
	explicit TimeCounter(int duration, std::string type = "sec") : time_(duration), sec_(true) {
		sec_ = type == "ms" ? false : true;
	}
	void run() {
		if (sec_)
			std::this_thread::sleep_for(std::chrono::seconds(time_));
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(time_));
	}
	void run_ms(int duration) {
		sec_ = false;
		time_ = duration;
		run();
	}
	void run_sec(int duration = 1) {
		sec_ = true;
		time_ = duration;
		run();
	}
	void count_sec(int duration) {
		if (duration < 1) return;
		do {
			progress(duration);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		} while (duration-- > 0);
	}
private:
	int  time_;
	bool sec_;
	void progress(int progress) {
		static int last_len = 0;
		std::ostringstream oss;
		oss << "TimeCounter: " << progress;
		last_len = static_cast<int>(oss.str().size());
		std::cout << "\r" << std::string(last_len + progress / 10 + 1, ' ') << "\r";
		std::cout << oss.str();
		std::cout.flush();
		if (progress == 0) {
			std::cout << "\r" << std::string(last_len + progress / 10 + 1, ' ') << "\r";
			std::cout.flush();
		}
	}

};

} // namespace testlog

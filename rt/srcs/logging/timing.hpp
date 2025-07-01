#pragma once
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>

namespace logging {

class Timer {
public:
	using  clock_t = std::chrono::steady_clock;
	using  time_point = clock_t::time_point;
	using  microseconds = std::chrono::microseconds;
	struct TimeEntry {
		std::string label;
		microseconds duration;
	};

	Timer() = default;
	Timer& start() {
		start_time_ = clock_t::now();
		return *this;
	}
	Timer& save_time(const std::string& label = "Elapsed time: ") {
		auto now = clock_t::now();
		auto duration = std::chrono::duration_cast<microseconds>(now - start_time_);
		times_.push_back({ label, duration });
		return *this;
	}
	Timer& output(std::ostream& os, const std::string& suffix = "\n") {
		for (const auto& entry : times_) {
			os << std::left << entry.label;
			os << format_duration(entry.duration);
			os << suffix;
			times_.clear();
		}
		return *this;
	}
	std::string output(const std::string& suffix = "\n") {
		std::ostringstream os;
		output(os, suffix);
		return os.str();
	}
	static std::string format_duration(microseconds us) {
		using namespace std::chrono;
		
		auto d  = duration_cast<days>(us);         us -= d;
		auto h  = duration_cast<hours>(us);        us -= h;
		auto m  = duration_cast<minutes>(us);      us -= m;
		auto s  = duration_cast<seconds>(us);      us -= s;
		auto ms = duration_cast<milliseconds>(us); us -= ms;
		auto us_final = us.count();
		
		std::ostringstream os;
		if (d.count() > 0)    os << d.count() << "d:";
		if (h.count() > 0 || d.count() > 0) os << h.count() << "h:";
		if (m.count() > 0 || h.count() > 0) os << m.count() << "m:";
		if (s.count() > 0 || m.count() > 0) os << s.count() << "s:";
		if (ms.count() > 0 || s.count() > 0) os << ms.count() << "ms:";
		if (us_final > 0 || ms.count() > 0)  os << us_final << "μs";
		
		return os.str();
	}
private:
	std::vector<TimeEntry> times_;
	time_point start_time_;
};


class ScopedTimer {
	using clock_t = Timer::clock_t;
	using time_point = Timer::time_point;
	
	std::string label;
	time_point start;
	
public:
	explicit ScopedTimer(const std::string& label = "Elapsed time")
	: label(label), start(clock_t::now()) {}
	
	~ScopedTimer() {
		auto end = clock_t::now();
		auto duration = std::chrono::duration_cast<Timer::microseconds>(end - start);
		std::cout << std::left << label << ": "
		<< Timer::format_duration(duration) << "\n";
	}
};


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

} // namespace logging

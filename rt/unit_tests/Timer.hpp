#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

class Timer {
public:
	using clock_t = std::chrono::steady_clock;
	using time_point = clock_t::time_point;
	using microseconds = std::chrono::microseconds;

private:
	time_point start_time;

	struct TimeEntry {
		std::string label;
		microseconds duration;
	};

	std::vector<TimeEntry> times;

public:
	Timer() = default;

	void start() {
		start_time = clock_t::now();
	}

	void save_time(const std::string& label = "Elapsed time") {
		auto now = clock_t::now();
		auto duration = std::chrono::duration_cast<microseconds>(now - start_time);
		times.push_back({ label, duration });
	}

	void output() {
		for (const auto& entry : times) {
			std::cout << std::left << entry.label << ": ";
			std::cout << format_duration(entry.duration) << "\n";
			times.clear();
		}
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

#endif // TIMER_HPP

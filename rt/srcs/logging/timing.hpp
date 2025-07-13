#pragma once
#include <ostream>
#include <string_view>
#include <chrono>
#include <vector>

namespace logging {

using os_t         = std::ostream;
using sv_t         = std::string_view;
using clock_t      = std::chrono::steady_clock;
using time_point_t = clock_t::time_point;
using microsec_t   = std::chrono::microseconds;

inline os_t& write_duration(os_t& os, microsec_t us) {
    using namespace std::chrono;
    auto d  = duration_cast<days>(us);         us -= d;
    auto h  = duration_cast<hours>(us);        us -= h;
    auto m  = duration_cast<minutes>(us);      us -= m;
    auto s  = duration_cast<seconds>(us);      us -= s;
    auto ms = duration_cast<milliseconds>(us); us -= ms;
    auto us_final = us.count();
    if (d.count()  > 0)                   os << d.count()  << "d:";
    if (h.count()  > 0 || d.count() > 0)  os << h.count()  << "h:";
    if (m.count()  > 0 || h.count() > 0)  os << m.count()  << "m:";
    if (s.count()  > 0 || m.count() > 0)  os << s.count()  << "s:";
    if (ms.count() > 0 || s.count() > 0)  os << ms.count() << "ms:";
    if (us_final   > 0 || ms.count() > 0) os << us_final   << "μs";
    return os;
}

class Timer {
public:
	struct TimeEntry {
		sv_t       label;
        microsec_t duration;
	};

	Timer() = default;

    [[nodiscard]] Timer& start() noexcept {
		start_time_ = clock_t::now();
		return *this;
	}

    [[nodiscard]] Timer& save_time(sv_t label = "Elapsed time: ") noexcept {
		auto now = clock_t::now();
		auto duration = std::chrono::duration_cast<microsec_t>(now - start_time_);
		times_.push_back({ label, duration });
		return *this;
	}

    os_t& write(os_t& os, sv_t suffix = "\n") noexcept {
		for (const auto& entry : times_) {
			os << entry.label;
            write_duration(os, entry.duration);
			os << suffix;
		}
        times_.clear();
		return os;
	}

private:
	std::vector<TimeEntry> times_;
    time_point_t           start_time_;
};

inline os_t& operator<<(os_t& os, Timer& timer) noexcept {
    return timer.write(os);
}

class ScopedTimer {
    os_t&        os;
	sv_t         label;
	time_point_t start;

public:
	explicit ScopedTimer(os_t& os_, sv_t label_ = "Elapsed time: ")
	: os(os_), label(label_), start(clock_t::now()) {}

	~ScopedTimer() noexcept {
        os << label;
        write_duration(os, std::chrono::duration_cast<microsec_t>(clock_t::now() - start));
        os << "\n";
	}
};

} // namespace logging

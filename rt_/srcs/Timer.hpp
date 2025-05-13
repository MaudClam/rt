#ifndef TIMER_HPP
# define TIMER_HPP

# include <iostream>
# include <chrono>


class Timer {
	std::chrono::time_point<std::chrono::steady_clock> _start;
	long _ms;
	bool _on;
public:
	Timer(void) : _start(), _ms(0), _on(false) {}
	Timer(const Timer& other) : _start(other._start), _on(other._on) {}
	~Timer(void) {}
	inline void on(void) { _on = true; }
	inline void off(void) { _on = false; }
	inline void start(void) {
		if (_on)
			_start = std::chrono::high_resolution_clock::now();
	}
	inline void read(void) {
		if (_on)
			_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now() - _start).count();
	}
	inline void output(std::string hint = "") {
		if (!_on)
			return;
		std::ostringstream os;
		read();
		long d = _ms / 86400000; _ms -= (d * 86400000);
		long h = _ms / 3600000; _ms -= (h * 3600000);
		long m = _ms / 60000; _ms -= (m * 60000);
		long s = _ms / 1000; _ms -= (s * 1000);
		if (!hint.empty()) os << hint << ": ";
		if (d) os << d << "d:";
		if (d || h) os << h << "h:";
		if (d || h || m) os << m << "m:";
		if (d || h || m || s) os << s << "s:";
		if (d || h || m || s || _ms) os << _ms << "ms";
		_ms = 0;
	}
};


#endif /* TIMER_HPP */

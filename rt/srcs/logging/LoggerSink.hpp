#pragma once
#include "logging.hpp"


namespace logging {

class LoggerSink {
public:
    enum class LogLevel { Info, Warning, Error };

    std::ostream& stream(LogLevel level) {
        if (use_file && file)
            return *file;
        switch (level) {
            case LogLevel::Info:    return out;
            case LogLevel::Warning: return err;
            case LogLevel::Error:   return err;
		}
        return out;
    }
    void set_file_output(std::ostream& f) {
        file = &f;
        use_file = true;
    }
    void disable_ansi() { ansi_enabled = false; }
    void enable_ansi()  { ansi_enabled = true;  }
    bool ansi() const { return ansi_enabled; }
    
private:
    std::ostream& out = std::cout;
    std::ostream& err = std::cerr;
    std::ostream* file = nullptr;
    bool          use_file = false;
    bool          ansi_enabled = true;
};

} // namespace logging

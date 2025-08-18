#include "config.hpp"
#include <filesystem>
#include <string>
#include "logging/logger.hpp"

#if defined(_WIN32)
# include <windows.h>
#elif defined(__APPLE__)
# include <mach-o/dyld.h>
#elif defined(__linux__)
# include <unistd.h>
# include <limits.h>
#endif

namespace rt {

std::filesystem::path get_exec_path() {
    char buffer[1024] = {};

#if defined(_WIN32)
    DWORD len = GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
    return std::filesystem::path(std::string(buffer, len));
# elif defined(__APPLE__)
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0)
        return std::filesystem::canonical(buffer);
# elif defined(__linux__)
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len > 0) {
        buffer[len] = '\0';
        return std::filesystem::canonical(buffer);
    }
#endif

    return {};
}

    Config     config;
    std::mutex log_global_mutex;
}

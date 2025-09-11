#pragma once
#include <cstddef>

namespace common {

void raw_write_stdout(const char* p, size_t n) noexcept;
void raw_write_stderr(const char* p, size_t n) noexcept;

} // namespace common

#if defined(_WIN32)
  #ifndef NOMINMAX
  #  define NOMINMAX
  #endif
  #ifndef WIN32_LEAN_AND_MEAN
  #  define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  inline void common::raw_write_stderr(const char* p, size_t n) noexcept {
    DWORD w = 0; ::WriteFile(::GetStdHandle(STD_ERROR_HANDLE), p, (DWORD)n, &w, nullptr);
  }
  inline void common::raw_write_stdout(const char* p, size_t n) noexcept {
    DWORD w = 0; ::WriteFile(::GetStdHandle(STD_OUTPUT_HANDLE), p, (DWORD)n, &w, nullptr);
  }
#else
  #include <unistd.h>
  inline void common::raw_write_stderr(const char* p, size_t n) noexcept {
    (void)::write(STDERR_FILENO, p, n);
  }
  inline void common::raw_write_stdout(const char* p, size_t n) noexcept {
    (void)::write(STDOUT_FILENO, p, n);
  }
#endif

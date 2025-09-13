#pragma once
#include <cstddef>
#include <string>
#include "traits.hpp"
#include "types.hpp"
#include "common_utils.hpp"
#include "return.hpp"

namespace common {

// No allocations, guaranteed NUL termination, null-safe C strings are accepted.
// Overflow policy - truncate+ellipsis.
template<size_t N>
struct RawBuffer {
    static_assert(N >= 5, "N too small");

    RawBuffer() = default;

    RawBuffer(const RawBuffer<N>& other) noexcept { *this = other; }
    RawBuffer(sv_t sv) noexcept { append(sv); }
    RawBuffer(const char* s) noexcept { append(s); }
    RawBuffer(char c) noexcept { append(c); }
    RawBuffer(int n) noexcept { append(n); }

    RawBuffer& operator=(const RawBuffer<N>& other) noexcept {
        if (this != &other) {
            std::memcpy(data_, other.data(), other.size());
            len_ = other.size();
            nullchar_ = other.nullchar();
            truncated_ = other.truncated();
        }
        return *this;
    }
    RawBuffer& operator=(sv_t sv) noexcept {reset(); return append(sv);}
    RawBuffer& operator=(const char* s) noexcept {reset(); return append(s);}
    RawBuffer& operator=(char c) noexcept {reset(); return append(c);}
    RawBuffer& operator=(int n) noexcept {reset(); return append(n);}

    template<typename... Args> requires (sizeof...(Args) > 1)
    RawBuffer(const Args&... args) noexcept {
        (append(args), ...);
        finalize_ellipsis_newline(false);
    }

    RawBuffer& clear() noexcept {
        reset();
        std::memset(data_ + 1, '\0', N - 1);
        return *this;
}

    RawBuffer& reset() noexcept {
        len_ = 0;
        nullchar_  = false;
        truncated_ = false;
        data_[0]   = '\0';
        return *this;
    }

    [[nodiscard]] Return status(sv_t prompt = {}) const noexcept {
        if (empty())
            return error("Empty string", prompt, false);
        if (truncated_)
            return error("String too long", prompt, false);
        if (nullchar_)
            return error("String contains null character", prompt, false);
        return ok();
    }

    [[nodiscard]] Return set(sv_t sv, sv_t prompt) noexcept {
        reset();
        append_(sv.data(), sv.size());
        finalize_ellipsis_newline(false);
        return status().set_prompt(prompt);
    }
    
    template<typename... Args>
    [[nodiscard]] Return load(const Args&... args) noexcept {
        reset();
        append(args...);
        finalize_ellipsis_newline(false);
        return status();
    }

    RawBuffer& append() = delete;
    template<typename... Args> requires (sizeof...(Args) >= 2)
    RawBuffer& append(const Args&... args) {(append(args), ...); return *this;}
    template<size_t M>
    RawBuffer& append(const RawBuffer<M>& buf) noexcept {return append_(buf.data(),buf.size());}
    RawBuffer& append(sv_t sv) noexcept {return append_(sv.data(), sv.size());}
    RawBuffer& append(const char* s) noexcept {return append_(s,safe_strlen(s));}
    RawBuffer& append(int n) noexcept {sv_t sv = to_sv(n); return append_(sv.data(),sv.size());}
    RawBuffer& append(char c) noexcept { return append_(&c, 1); }
    RawBuffer& append(bool b) noexcept {sv_t sv = as_sv(b); return append_(sv.data(),sv.size());}
    RawBuffer& append(const Return& status) noexcept {
        if (status.ok()) return *this;
        append(status.status);
        if (status.prompt.empty()) return *this;
        append(" '"); append(status.prompt); append('\'');
        return *this;
    }
    
    template <size_t M>
    RawBuffer& append(const std::bitset<M>& b) noexcept {
        const size_t need = M;
        const size_t room = (N - 1) - len_;
        const size_t take = need <= room ? need : room;
        if (take < need) truncated_ = true;
        for (size_t i = 0; i < take; ++i) {
            const bool bit = b.test(M - 1 - i);
            data_[len_ + i] = bit ? '1' : '0';
        }
        len_ += take;
        data_[len_] = '\0';
        return *this;
    }

    template<class T> requires traits::AdlAsSv<T>
    RawBuffer& append(const T& x) noexcept(noexcept(as_sv(x))) {
        sv_t sv = as_sv(x);
        return append_(sv.data(), sv.size());
    }

    template<typename... Args> requires (sizeof...(Args) > 0)
    RawBuffer& add_prefix(sv_t pre_pref, const Args&... pre) noexcept {
        if (len_ == 0) return *this;
        RawBuffer<N> tmp;
        if (!pre_pref.empty() && view().starts_with(pre_pref)) {
            tmp.append(pre_pref, pre..., safe_substr(view(), pre_pref.size()));
        } else {
            tmp.append(pre...);
            if (tmp.empty()) return *this;
            tmp.append(*this);
        }
        tmp.finalize_ellipsis_newline(true);
        return *this = tmp.view();
    }

    template<typename... Args> requires (sizeof...(Args) > 0)
    RawBuffer& add_prefix_lines(const Args&... pre) noexcept {
        if (len_ == 0) return *this;
        RawBuffer<N> pref(pre...);
        if (pref.empty()) return *this;
        RawBuffer<N> out{};
        bool sol = true;
        for (size_t i = 0; i < len_; ++i) {
            if (sol && data_[i] != '\n') {
                out.append(pref);
                sol = false;
            }
            out.append(data_[i]);
            if (data_[i] == '\n') sol = true;
        }
        out.finalize_ellipsis_newline(true);
        return *this = out.view();
    }

    void finalize_ellipsis_newline(bool add_newline) noexcept {
        if (!truncated_) return;
        if (add_newline) {
            if (len_ >= 4) {
                data_[len_-4]='.';
                data_[len_-3]='.';
                data_[len_-2]='.';
                data_[len_-1]='\n';
            }
        } else {
            if (len_ >= 3) {
                data_[len_-3]='.';
                data_[len_-2]='.';
                data_[len_-1]='.';
            }
        }
    }

    [[nodiscard]] sv_t view() const noexcept {return {data_, len_};}
    [[nodiscard]] const char* c_str() const noexcept {return data_;}
    [[nodiscard]] const char* data()  const noexcept {return data_;}
    [[nodiscard]] static constexpr size_t capacity() noexcept {return N - 1;}
    [[nodiscard]] size_t size() const noexcept {return len_;}
    [[nodiscard]] size_t remaining() const noexcept {return (N - 1) - len_;}
    [[nodiscard]] bool empty() const noexcept {return !len_;}
    [[nodiscard]] bool truncated() const noexcept {return truncated_;}
    [[nodiscard]] bool nullchar() const noexcept {return nullchar_;}

    [[nodiscard]] bool operator==(sv_t sv) const noexcept {return view() == sv;}
    [[nodiscard]] bool operator!=(sv_t sv) const noexcept {return (view() != sv);}
    [[nodiscard]] bool operator==(const char* s) const noexcept {return view() == s;}
    [[nodiscard]] bool operator!=(const char* s) const noexcept {return view() != s;}
    [[nodiscard]] bool operator==(int n) const noexcept {return view() == to_sv(n);}
    [[nodiscard]] bool operator!=(int n) const noexcept {return view() != to_sv(n);}
    [[nodiscard]] bool operator==(const RawBuffer& other) const noexcept {
        return view() == other.view() &&
               truncated_ = other.truncated_ &&
               nullchar_ = other.nullchar_;
    }
    [[nodiscard]] bool operator!=(const RawBuffer& other) const noexcept {
        return !(*this == other);
    }

    os_t& write(os_t& os) const { return os.write(data_, len_); }

private:
    char   data_[N]{};
    size_t len_ = 0;
    bool   nullchar_  = false;
    bool   truncated_ = false;

    RawBuffer& append_(const char* p, size_t n) noexcept {
        if (!p || n == 0) return *this;
        const size_t room = (N - 1) - len_;
        const size_t take = (n <= room ? n : room);
        if (take) {
            if (std::memchr(p, '\0', n)) {
                std::memset(data_ + len_, '\0', room);
                nullchar_ = true;
            }
            std::memcpy(data_ + len_, p, take);
            len_ += take;
        }
        if (take < n) truncated_ = true;
        data_[len_] = '\0';
        return *this;
    }
};

template<size_t N>
inline os_t& operator<<(os_t& os, const RawBuffer<N>& buf) {
    return buf.write(os);
}

template<size_t N = 256, typename Tag = struct tl_default_tag>
inline RawBuffer<N>& tl_raw_buffer(bool reset = true) noexcept {
    thread_local RawBuffer<N> buf{};
    if (reset) buf.reset();
    return buf;
}

template<size_t N = 256, typename Tag = struct tl_default_tag, typename... Args>
RawBuffer<N>& build_tl_raw_buffer(const Args&... args) noexcept {
    auto& buf = tl_raw_buffer<N>();
    (buf.append(args), ...);
    buf.finalize_ellipsis_newline(false);
    return buf;
}

template<typename Tag = struct tl_default_tag>
[[nodiscard]] oss_t& tl_buffer(bool restore = true) {
    static thread_local oss_t oss{};
    oss.str("");
    oss.clear();
    if (restore) {
        oss.flags(std::ios_base::fmtflags{});
        oss.precision(6);
    }
    return oss;
}

}//namespace common

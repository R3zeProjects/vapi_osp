#ifndef VAPI_CORE_BUILD_INFO_HPP
#define VAPI_CORE_BUILD_INFO_HPP

/** @file build_info.hpp
 *  @brief Build-time identity: author hash, obfuscation helpers (ObfString, ObfPassthrough).
 *  Include this where author or obf types are needed; types.hpp no longer defines them. */

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace vapi {

namespace detail {
    constexpr unsigned obf_key = 0x9Au;
    template<std::size_t N>
    constexpr std::array<char, N> obf_xor(const char (&s)[N], unsigned key) {
        std::array<char, N> r{};
        for (std::size_t i = 0; i < N - 1; ++i)
            r[i] = static_cast<char>(static_cast<unsigned char>(s[i]) ^ (key + static_cast<unsigned>(i)));
        r[N - 1] = '\0';
        return r;
    }
}

template<std::size_t N>
struct ObfString {
    std::array<char, N> data;
    constexpr ObfString(const std::array<char, N>& d) : data(d) {}
    std::string decode() const {
        std::string out;
        out.reserve(N - 1);
        for (std::size_t i = 0; i < N - 1; ++i)
            out += static_cast<char>(static_cast<unsigned char>(data[i]) ^ (detail::obf_key + static_cast<unsigned>(i)));
        return out;
    }
};

struct ObfPassthrough {
    std::string s;
    explicit ObfPassthrough(const char* p) : s(p) {}
    std::string decode() const { return s; }
};

/** Author identity (opaque hash). */
inline constexpr std::string_view author =
    "bcc2eecba5ed7cb5bfd489f52db02fe344297ec4de521e73b2fab09abc5fe896"
    "92998ac284d6a441d51669bb06cd8dea5671810fb1973bc7fa8f954d32053545";

} // namespace vapi

#endif

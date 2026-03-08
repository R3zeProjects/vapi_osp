#ifndef VAPI_CORE_STRING_UTILS_HPP
#define VAPI_CORE_STRING_UTILS_HPP

#include "core/types.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <format>
#include <algorithm>
#include <charconv>
#include <optional>
#include <cctype>

namespace vapi::str {

template<typename... Args>
[[nodiscard]] std::string format(std::format_string<Args...> fmt, Args&&... args) {
    return std::format(fmt, std::forward<Args>(args)...);
}

[[nodiscard]] inline std::vector<std::string_view> split(std::string_view s, char d, bool skipEmpty = true) {
    std::vector<std::string_view> r;
    usize start = 0;
    for (usize i = 0; i <= s.size(); ++i) {
        if (i == s.size() || s[i] == d) {
            auto part = s.substr(start, i - start);
            if (!skipEmpty || !part.empty()) r.push_back(part);
            start = i + 1;
        }
    }
    return r;
}

[[nodiscard]] inline std::vector<std::string_view> split(std::string_view s, std::string_view d, bool skipEmpty = true) {
    std::vector<std::string_view> r;
    usize start = 0, pos;
    while ((pos = s.find(d, start)) != std::string_view::npos) {
        auto part = s.substr(start, pos - start);
        if (!skipEmpty || !part.empty()) r.push_back(part);
        start = pos + d.size();
    }
    auto last = s.substr(start);
    if (!skipEmpty || !last.empty()) r.push_back(last);
    return r;
}

[[nodiscard]] inline std::string_view trimLeft(std::string_view s) {
    auto p = s.find_first_not_of(" \t\n\r\f\v");
    return p == std::string_view::npos ? "" : s.substr(p);
}
[[nodiscard]] inline std::string_view trimRight(std::string_view s) {
    auto p = s.find_last_not_of(" \t\n\r\f\v");
    return p == std::string_view::npos ? "" : s.substr(0, p + 1);
}
[[nodiscard]] inline std::string_view trim(std::string_view s) { return trimRight(trimLeft(s)); }

[[nodiscard]] inline std::string toLower(std::string_view s) {
    std::string r(s);
    std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return r;
}
[[nodiscard]] inline std::string toUpper(std::string_view s) {
    std::string r(s);
    std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c){ return static_cast<char>(std::toupper(c)); });
    return r;
}

[[nodiscard]] inline u32 decodeUtf8(const char* data, u32 size, u32& pos) {
    if (pos >= size) return 0;
    const u8 byte = static_cast<u8>(data[pos]);
    u32 cp = 0;
    u32 len = 1;
    if (byte < 0x80) {
        cp = byte;
        len = 1;
    } else if ((byte & 0xE0) == 0xC0) {
        cp = byte & 0x1F;
        len = 2;
    } else if ((byte & 0xF0) == 0xE0) {
        cp = byte & 0x0F;
        len = 3;
    } else if ((byte & 0xF8) == 0xF0) {
        cp = byte & 0x07;
        len = 4;
    } else {
        pos++;
        return static_cast<u32>('?');
    }
    if (pos + len > size) {
        pos++;
        return static_cast<u32>('?');
    }
    for (u32 i = 1; i < len; i++) {
        const u8 cont = static_cast<u8>(data[pos + i]);
        if ((cont & 0xC0) != 0x80) {
            pos++;
            return static_cast<u32>('?');
        }
        cp = (cp << 6) | (cont & 0x3F);
    }
    pos += len;
    return cp;
}

[[nodiscard]] inline bool contains(std::string_view s, std::string_view sub) { return s.find(sub) != std::string_view::npos; }
[[nodiscard]] inline bool startsWith(std::string_view s, std::string_view p) { return s.size() >= p.size() && s.substr(0, p.size()) == p; }
[[nodiscard]] inline bool endsWith(std::string_view s, std::string_view x)   { return s.size() >= x.size() && s.substr(s.size()-x.size()) == x; }

[[nodiscard]] inline std::string replace(std::string_view s, std::string_view from, std::string_view to) {
    if (from.empty()) return std::string(s);
    std::string r; r.reserve(s.size());
    usize start = 0, pos;
    while ((pos = s.find(from, start)) != std::string_view::npos) {
        r.append(s.substr(start, pos - start));
        r.append(to);
        start = pos + from.size();
    }
    r.append(s.substr(start));
    return r;
}

template<typename C>
[[nodiscard]] std::string join(const C& parts, std::string_view sep) {
    std::string r; bool first = true;
    for (const auto& p : parts) { if (!first) r.append(sep); r.append(p); first = false; }
    return r;
}

template<typename T>
[[nodiscard]] std::optional<T> parse(std::string_view s) {
    s = trim(s); T v{};
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec == std::errc() && ptr == s.data() + s.size()) return v;
    return std::nullopt;
}

template<>
[[nodiscard]] inline std::optional<f32> parse<f32>(std::string_view s) {
    s = trim(s);
    try { std::string ss(s); size_t pos; f32 v = std::stof(ss, &pos); if (pos == ss.size()) return v; }
    catch (...) {}
    return std::nullopt;
}

} // namespace vapi::str

#endif

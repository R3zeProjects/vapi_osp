#ifndef VAPI_CORE_MATH_UTILS_HPP
#define VAPI_CORE_MATH_UTILS_HPP

/** @file math_utils.hpp
 *  @brief Math constants, clamp/lerp/smoothstep, vec2/rect/color helpers, wrap, distance, rotate. */

#include "core/types.hpp"
#include <cmath>
#include <numbers>
#include <algorithm>

namespace vapi::math {

inline constexpr f32 PI         = std::numbers::pi_v<f32>;
inline constexpr f32 TWO_PI     = PI * 2.f;
inline constexpr f32 HALF_PI    = PI * 0.5f;
inline constexpr f32 DEG_TO_RAD = PI / 180.f;
inline constexpr f32 RAD_TO_DEG = 180.f / PI;
inline constexpr f32 EPSILON    = 1e-6f;

template<typename T>
[[nodiscard]] constexpr T clamp(T value, T lo, T hi) { return std::max(lo, std::min(value, hi)); }

template<typename T>
[[nodiscard]] constexpr T lerp(T a, T b, f32 t) { return a + static_cast<T>((b - a) * t); }

template<typename T>
[[nodiscard]] constexpr f32 inverseLerp(T a, T b, T v) {
    if (std::abs(static_cast<f32>(b - a)) < EPSILON) return 0.f;
    return static_cast<f32>(v - a) / static_cast<f32>(b - a);
}

template<typename T>
[[nodiscard]] constexpr T mapRange(T v, T inMin, T inMax, T outMin, T outMax) {
    return lerp(outMin, outMax, inverseLerp(inMin, inMax, v));
}

[[nodiscard]] constexpr f32 degToRad(f32 d) { return d * DEG_TO_RAD; }
[[nodiscard]] constexpr f32 radToDeg(f32 r) { return r * RAD_TO_DEG; }

[[nodiscard]] constexpr f32 smoothstep(f32 e0, f32 e1, f32 x) {
    if (e1 <= e0) return (x >= e1 ? 1.f : 0.f);
    f32 t = clamp((x - e0) / (e1 - e0), 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

[[nodiscard]] constexpr f32 smootherstep(f32 e0, f32 e1, f32 x) {
    if (e1 <= e0) return (x >= e1 ? 1.f : 0.f);
    f32 t = clamp((x - e0) / (e1 - e0), 0.f, 1.f);
    return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
}

template<typename T>
[[nodiscard]] constexpr s32 sign(T v) { return (T(0) < v) - (v < T(0)); }

[[nodiscard]] constexpr bool approxEqual(f32 a, f32 b, f32 eps = EPSILON) { return std::abs(a - b) < eps; }

[[nodiscard]] f32 wrap(f32 v, f32 max);
[[nodiscard]] f32 wrap(f32 v, f32 min, f32 max);

[[nodiscard]] f32 distance(point a, point b);
[[nodiscard]] constexpr f32 distanceSq(point a, point b) { auto d = b - a; return d.x*d.x + d.y*d.y; }
[[nodiscard]] constexpr vec2 lerp(vec2 a, vec2 b, f32 t) { return {lerp(a.x, b.x, t), lerp(a.y, b.y, t)}; }
[[nodiscard]] constexpr vec2 clamp(vec2 v, vec2 lo, vec2 hi) {
    return {clamp(v.x, lo.x, hi.x), clamp(v.y, lo.y, hi.y)};
}
[[nodiscard]] constexpr vec2 reflect(vec2 d, vec2 n) { return d - n * (2.f * d.dot(n)); }
[[nodiscard]] vec2 rotate(vec2 v, f32 a);
[[nodiscard]] f32 angle(vec2 v);

[[nodiscard]] constexpr rect lerp(const rect& a, const rect& b, f32 t) {
    return {lerp(a.min, b.min, t), lerp(a.max, b.max, t)};
}
[[nodiscard]] constexpr rect intersection(const rect& a, const rect& b) {
    return {{std::max(a.min.x, b.min.x), std::max(a.min.y, b.min.y)},
            {std::min(a.max.x, b.max.x), std::min(a.max.y, b.max.y)}};
}
[[nodiscard]] constexpr rect unionRect(const rect& a, const rect& b) {
    return {{std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y)},
            {std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y)}};
}

[[nodiscard]] constexpr color4 clamp(color4 c, f32 lo = 0.f, f32 hi = 1.f) {
    return {clamp(c.r, lo, hi), clamp(c.g, lo, hi), clamp(c.b, lo, hi), clamp(c.a, lo, hi)};
}
[[nodiscard]] color4 hsvToRgb(f32 h, f32 s, f32 v, f32 a = 1.f);

} // namespace vapi::math

#endif // VAPI_CORE_MATH_UTILS_HPP

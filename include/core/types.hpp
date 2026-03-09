#ifndef VAPI_TYPES_HPP
#define VAPI_TYPES_HPP

/** @file types.hpp
 *  @brief Core type aliases, geometric and color types, Result, Handle.
 *
 *  ## Стиль именования (naming)
 *  - **Геттеры**: для свойств-значений без побочных эффектов предпочтительно имя без префикса get
 *    (например width(), height(), value(), ok()), если имя не совпадает с типом или неочевидно — можно getX.
 *  - **Предикаты**: bool-геттеры — без get: isValid(), ok(), empty(); при необходимости hasX(), isX().
 *  - **Фабрики/статические конструкторы**: fromX(), null(), white() и т.д. Рефакторинг существующего API не обязателен. */

#include <cstdint>
#include <cstddef>
#include <string_view>
#include <string>
#include <expected>
#include <vector>
#include <chrono>
#include <source_location>
#include <array>

namespace vapi
{
    using u8  = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using s8  = std::int8_t;
    using s16 = std::int16_t;
    using s32 = std::int32_t;
    using s64 = std::int64_t;

    using f32  = float;
    using f64  = double;
    using fext = long double;

    using sptr  = std::intptr_t;
    using uptr  = std::uintptr_t;
    using usize = std::size_t;
    using ssize = sptr;

    using WindowId = u32;

    /** @brief Код ошибки (0 = успех). Используется в Result<T> и errorMessage(). */
    struct Error {
        u32 code{0};

        constexpr Error() = default;
        constexpr explicit Error(u32 v) : code(v) {}

        [[nodiscard]] constexpr u32  value() const noexcept { return code; }
        [[nodiscard]] constexpr bool ok()    const noexcept { return code == 0; }

        constexpr bool operator==(Error other) const noexcept { return code == other.code; }
        constexpr bool operator!=(Error other) const noexcept { return code != other.code; }

        explicit constexpr operator bool() const { return !ok(); }
        explicit constexpr operator u32()  const { return code; }
    };

    /** @brief Результат операции: либо значение типа T, либо Error. Алиас std::expected<T, Error>. */
    template<typename T>
    using Result = std::expected<T, Error>;

    /** @brief Дескриптор ресурса с индексом и поколением (generation) для валидации. */
    template<typename T>
    struct Handle {
        u32 index{0};
        u32 generation{0};

        constexpr bool operator==(const Handle&) const = default;
        [[nodiscard]] constexpr bool isValid() const noexcept { return index != 0; }
        explicit constexpr operator bool() const noexcept { return isValid(); }

        [[nodiscard]] static constexpr Handle null() noexcept { return {0, 0}; }
    };

    template<typename T>
    class HandlePool {
        std::vector<u32> m_generations;
        std::vector<u32> m_freeIndices;
    public:
        [[nodiscard]] Handle<T> allocate() {
            u32 index;
            if (!m_freeIndices.empty()) {
                index = m_freeIndices.back();
                m_freeIndices.pop_back();
                m_generations[index - 1]++;
            } else {
                index = static_cast<u32>(m_generations.size()) + 1;
                m_generations.push_back(0);
            }
            return {index, m_generations[index - 1]};
        }
        void free(Handle<T> handle) {
            if (handle.index > 0 && handle.index <= static_cast<u32>(m_generations.size()) &&
                m_generations[handle.index - 1] == handle.generation) {
                m_generations[handle.index - 1]++;
                m_freeIndices.push_back(handle.index);
            }
        }
        [[nodiscard]] bool isValid(Handle<T> handle) const {
            if (handle.index == 0 || handle.index > static_cast<u32>(m_generations.size()))
                return false;
            return m_generations[handle.index - 1] == handle.generation;
        }
        [[nodiscard]] usize activeCount() const {
            return m_generations.size() - m_freeIndices.size();
        }
        void clear() { m_generations.clear(); m_freeIndices.clear(); }
        void reserve(usize capacity) {
            m_generations.reserve(capacity);
            m_freeIndices.reserve(capacity);
        }
    };

    enum class Easing : u8 {
        Linear,
        EaseInQuad,    EaseOutQuad,    EaseInOutQuad,
        EaseInCubic,   EaseOutCubic,   EaseInOutCubic,
        EaseInQuart,   EaseOutQuart,   EaseInOutQuart,
        EaseInQuint,   EaseOutQuint,   EaseInOutQuint,
        EaseInSine,    EaseOutSine,    EaseInOutSine,
        EaseInExpo,    EaseOutExpo,    EaseInOutExpo,
        EaseInCirc,    EaseOutCirc,    EaseInOutCirc,
        EaseInBack,    EaseOutBack,    EaseInOutBack,
        EaseInElastic, EaseOutElastic, EaseInOutElastic,
        EaseInBounce,  EaseOutBounce,  EaseInOutBounce
    };

    struct SizeElement
    {
        u32 width{0};
        u32 height{0};

        SizeElement() = default;
        SizeElement(const SizeElement&) = default;
        constexpr SizeElement(u32 w, u32 h): width(w), height(h) {}

        SizeElement& operator=(const SizeElement& other) = default;

        constexpr bool operator==(const SizeElement& other) const
        {
            return width == other.width && height == other.height;
        }

        constexpr SizeElement operator*(u32 scalar) const
        {
            return SizeElement(width * scalar, height * scalar);
        }
        constexpr SizeElement operator+(u32 scalar) const
        {
            return SizeElement(width + scalar, height + scalar);
        }
        constexpr SizeElement operator-(u32 scalar) const
        {
            const u32 w = width >= scalar ? width - scalar : 0u;
            const u32 h = height >= scalar ? height - scalar : 0u;
            return SizeElement(w, h);
        }
        constexpr SizeElement operator/(u32 scalar) const
        {
            if (scalar == 0u) return SizeElement(0u, 0u);
            return SizeElement(width / scalar, height / scalar);
        }

        [[nodiscard]] constexpr Result<SizeElement> tryDivide(u32 scalar) const
        {
            if (scalar == 0u) return std::unexpected(Error(0x100A));
            return SizeElement(width / scalar, height / scalar);
        }
    };

    inline constexpr SizeElement operator*(u32 scalar, const SizeElement& s)
    {
        return s * scalar;
    }

    inline constexpr SizeElement operator+(u32 scalar, const SizeElement& s)
    {
        return s + scalar;
    }

    enum class AxisDirection : u8 {
        Right = 0, Left  = 1, Down  = 0, Up    = 1
    };

    struct CoordSystem {
        AxisDirection xAxis{AxisDirection::Right};
        AxisDirection yAxis{AxisDirection::Down};
    };

    struct vec2 {
        f32 x{0.f}, y{0.f};

        constexpr vec2() = default;
        constexpr vec2(f32 x_, f32 y_) : x(x_), y(y_) {}

        vec2& operator=(const vec2&) = default;

        constexpr vec2 operator+() const { return *this; }
        constexpr vec2 operator-() const { return {-x, -y}; }
        constexpr vec2 operator+(vec2 o) const { return {x + o.x, y + o.y}; }
        constexpr vec2 operator-(vec2 o) const { return {x - o.x, y - o.y}; }
        constexpr vec2 operator*(f32 s) const { return {x * s, y * s}; }
        constexpr vec2 operator+(f32 s) const { return {x + s, y + s}; }
        constexpr vec2 operator-(f32 s) const {
            const f32 x_ = (x >= s) ? (x - s) : 0.f;
            const f32 y_ = (y >= s) ? (y - s) : 0.f;
            return {x_, y_};
        }
        constexpr vec2 operator/(f32 s) const {
            if (s == 0.f) return {0.f, 0.f};
            return {x / s, y / s};
        }

        [[nodiscard]] constexpr Result<vec2> tryDivide(f32 s) const {
            if (s == 0.f) return std::unexpected(Error(0x100A));
            return vec2(x / s, y / s);
        }

        constexpr vec2& operator+=(vec2 o) { x += o.x; y += o.y; return *this; }
        constexpr vec2& operator-=(vec2 o) { x -= o.x; y -= o.y; return *this; }
        constexpr vec2& operator*=(f32 s) { x *= s; y *= s; return *this; }
        constexpr vec2& operator/=(f32 s) { x /= s; y /= s; return *this; }

        [[nodiscard]] f32 length()   const;
        [[nodiscard]] f32 lengthSq() const { return x*x + y*y; }
        [[nodiscard]] vec2 normalized() const;
        [[nodiscard]] f32 dot(vec2 o) const { return x*o.x + y*o.y; }

        constexpr bool operator==(const vec2&) const = default;
    };

    inline constexpr vec2 operator*(f32 s, vec2 v) { return v * s; }
    inline constexpr vec2 operator+(f32 s, vec2 v) { return v + s; }

    struct vec3 {
        f32 x{0.f}, y{0.f}, z{0.f};
        constexpr vec3() = default;
        constexpr vec3(f32 x_, f32 y_, f32 z_) : x(x_), y(y_), z(z_) {}
        constexpr vec3 operator+(vec3 o) const { return {x + o.x, y + o.y, z + o.z}; }
        constexpr vec3 operator-(vec3 o) const { return {x - o.x, y - o.y, z - o.z}; }
        constexpr vec3 operator*(f32 s) const { return {x * s, y * s, z * s}; }
        constexpr vec3 operator/(f32 s) const {
            if (s == 0.f) return {0.f, 0.f, 0.f};
            return {x / s, y / s, z / s};
        }
        [[nodiscard]] f32 lengthSq() const { return x*x + y*y + z*z; }
        constexpr bool operator==(const vec3&) const = default;
    };
    inline constexpr vec3 operator*(f32 s, vec3 v) { return v * s; }

    struct vec4 {
        f32 x{0.f}, y{0.f}, z{0.f}, w{1.f};
        constexpr vec4() = default;
        constexpr vec4(f32 x_, f32 y_, f32 z_, f32 w_ = 1.f) : x(x_), y(y_), z(z_), w(w_) {}
        constexpr vec4 operator+(vec4 o) const { return {x + o.x, y + o.y, z + o.z, w + o.w}; }
        constexpr vec4 operator-(vec4 o) const { return {x - o.x, y - o.y, z - o.z, w - o.w}; }
        constexpr vec4 operator*(f32 s) const { return {x * s, y * s, z * s, w * s}; }
        constexpr vec4 operator/(f32 s) const {
            if (s == 0.f) return {0.f, 0.f, 0.f, 0.f};
            return {x / s, y / s, z / s, w / s};
        }
        constexpr bool operator==(const vec4&) const = default;
    };
    inline constexpr vec4 operator*(f32 s, vec4 v) { return v * s; }

    using point = vec2;

    struct rect {
        point min{0.f, 0.f};
        point max{1.f, 1.f};

        constexpr rect() = default;
        constexpr rect(f32 x, f32 y, f32 w, f32 h)
            : min{x, y}, max{x + w, y + h} {}
        constexpr rect(point min_, point max_)
            : min(min_), max(max_) {}

        rect& operator=(const rect&) = default;

        constexpr rect operator*(f32 s) const {
            const point c = center();
            const f32 w = width() * s, h = height() * s;
            return {c.x - w * 0.5f, c.y - h * 0.5f, w, h};
        }
        constexpr rect operator+(f32 amount) const { return expand(amount); }
        constexpr rect operator-(f32 amount) const {
            const f32 w = width(), h = height();
            const f32 half = (amount * 0.5f);
            if (half >= w * 0.5f || half >= h * 0.5f) return {center().x, center().y, 0.f, 0.f};
            return expand(-amount);
        }
        constexpr rect operator/(f32 s) const {
            if (s == 0.f) return {min.x, min.y, 0.f, 0.f};
            return operator*(1.f / s);
        }

        [[nodiscard]] constexpr f32   width()  const { return max.x - min.x; }
        [[nodiscard]] constexpr f32   height() const { return max.y - min.y; }
        [[nodiscard]] constexpr point size()   const { return {width(), height()}; }
        [[nodiscard]] constexpr point center() const {
            return {(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f};
        }
        [[nodiscard]] constexpr bool contains(point p) const {
            return p.x >= min.x && p.x <= max.x &&
                   p.y >= min.y && p.y <= max.y;
        }
        [[nodiscard]] constexpr bool intersects(const rect& o) const {
            return !(max.x < o.min.x || min.x > o.max.x ||
                     max.y < o.min.y || min.y > o.max.y);
        }
        [[nodiscard]] constexpr rect expand(f32 amount) const {
            return {{min.x - amount, min.y - amount},
                    {max.x + amount, max.y + amount}};
        }
        [[nodiscard]] constexpr rect offset(point delta) const {
            return {min + delta, max + delta};
        }
    };

    inline constexpr rect operator*(f32 s, const rect& r) { return r * s; }
    inline constexpr rect operator+(f32 s, const rect& r) { return r + s; }

    struct hsl_t { f32 h{0.f}; f32 s{0.f}; f32 l{0.f}; };
    struct hsv_t { f32 h{0.f}; f32 s{0.f}; f32 v{0.f}; };
    struct cmyk_t { f32 c{0.f}; f32 m{0.f}; f32 y{0.f}; f32 k{0.f}; };

    struct color3;
    struct color4;

    struct color {
        f32 r{0.f}, g{0.f}, b{0.f};

        constexpr color() = default;
        constexpr color(f32 r_, f32 g_, f32 b_) : r(r_), g(g_), b(b_) {}

        color& operator=(const color&) = default;

        constexpr color operator*(f32 s) const { return {r * s, g * s, b * s}; }
        constexpr color operator+(f32 s) const { return {r + s, g + s, b + s}; }
        constexpr color operator-(f32 s) const {
            const f32 r_ = (r >= s) ? (r - s) : 0.f;
            const f32 g_ = (g >= s) ? (g - s) : 0.f;
            const f32 b_ = (b >= s) ? (b - s) : 0.f;
            return {r_, g_, b_};
        }
        constexpr color operator/(f32 s) const {
            if (s == 0.f) return {0.f, 0.f, 0.f};
            return {r / s, g / s, b / s};
        }

        [[nodiscard]] constexpr color3 toColor3() const;
        [[nodiscard]] constexpr color4 toColor4(f32 alpha = 1.f) const;
        [[nodiscard]] u32 toHex() const;
        [[nodiscard]] hsl_t toHSL() const;
        [[nodiscard]] hsv_t toHSV() const;
        [[nodiscard]] cmyk_t toCMYK() const;
    };

    struct color3 : color {
        constexpr color3() : color(0.f, 0.f, 0.f) {}
        constexpr color3(f32 r_, f32 g_, f32 b_) : color(r_, g_, b_) {}
        constexpr explicit color3(const color& c) : color(c.r, c.g, c.b) {}

        color3& operator=(const color3&) = default;

        constexpr color3 operator*(f32 s) const { return color3(r * s, g * s, b * s); }
        constexpr color3 operator+(f32 s) const { return color3(r + s, g + s, b + s); }
        constexpr color3 operator-(f32 s) const {
            const f32 r_ = (r >= s) ? (r - s) : 0.f;
            const f32 g_ = (g >= s) ? (g - s) : 0.f;
            const f32 b_ = (b >= s) ? (b - s) : 0.f;
            return color3(r_, g_, b_);
        }
        constexpr color3 operator/(f32 s) const {
            if (s == 0.f) return color3(0.f, 0.f, 0.f);
            return color3(r / s, g / s, b / s);
        }

        [[nodiscard]] constexpr color4 toColor4(f32 alpha = 1.f) const;
        [[nodiscard]] u32 toHex() const;
        [[nodiscard]] hsl_t toHSL() const;
        [[nodiscard]] hsv_t toHSV() const;
        [[nodiscard]] cmyk_t toCMYK() const;

        [[nodiscard]] static constexpr color3 fromHex(u32 hex) {
            return color3(f32((hex >> 16) & 0xFF) / 255.f,
                          f32((hex >>  8) & 0xFF) / 255.f,
                          f32( hex        & 0xFF) / 255.f);
        }
        [[nodiscard]] static color3 fromHSL(const hsl_t&);
        [[nodiscard]] static color3 fromHSV(const hsv_t&);
        [[nodiscard]] static color3 fromCMYK(const cmyk_t&);
    };

    struct color4 : color {
        f32 a{1.f};

        constexpr color4() : color(0.f, 0.f, 0.f), a(1.f) {}
        constexpr color4(f32 r_, f32 g_, f32 b_, f32 a_ = 1.f) : color(r_, g_, b_), a(a_) {}
        constexpr explicit color4(const color& c, f32 alpha = 1.f) : color(c), a(alpha) {}
        constexpr explicit color4(const color3& c, f32 alpha = 1.f) : color(c.r, c.g, c.b), a(alpha) {}

        color4& operator=(const color4&) = default;

        constexpr color4 operator*(f32 s) const { return {r * s, g * s, b * s, a * s}; }
        constexpr color4 operator+(f32 s) const { return {r + s, g + s, b + s, a + s}; }
        constexpr color4 operator-(f32 s) const {
            const f32 r_ = (r >= s) ? (r - s) : 0.f;
            const f32 g_ = (g >= s) ? (g - s) : 0.f;
            const f32 b_ = (b >= s) ? (b - s) : 0.f;
            const f32 a_ = (a >= s) ? (a - s) : 0.f;
            return {r_, g_, b_, a_};
        }
        constexpr color4 operator/(f32 s) const {
            if (s == 0.f) return {0.f, 0.f, 0.f, 0.f};
            return {r / s, g / s, b / s, a / s};
        }

        [[nodiscard]] constexpr color3 toColor3() const { return color3(r, g, b); }
        [[nodiscard]] constexpr color4 withAlpha(f32 alpha) const { return {r, g, b, alpha}; }
        [[nodiscard]] constexpr color4 lerp(const color4& to, f32 t) const {
            return { r + (to.r - r) * t, g + (to.g - g) * t,
                     b + (to.b - b) * t, a + (to.a - a) * t };
        }
        [[nodiscard]] u32 toHex() const;
        [[nodiscard]] hsl_t toHSL() const;
        [[nodiscard]] hsv_t toHSV() const;
        [[nodiscard]] cmyk_t toCMYK() const;

        [[nodiscard]] static constexpr color4 fromHex(u32 hex, f32 alpha = 1.f) {
            return { f32((hex >> 16) & 0xFF) / 255.f,
                     f32((hex >>  8) & 0xFF) / 255.f,
                     f32( hex        & 0xFF) / 255.f, alpha };
        }
        [[nodiscard]] static color4 fromHSL(const hsl_t&, f32 alpha = 1.f);
        [[nodiscard]] static color4 fromHSV(const hsv_t&, f32 alpha = 1.f);
        [[nodiscard]] static color4 fromCMYK(const cmyk_t&, f32 alpha = 1.f);

        static constexpr color4 white()       { return {1,1,1,1}; }
        static constexpr color4 black()       { return {0,0,0,1}; }
        static constexpr color4 red()         { return {1,0,0,1}; }
        static constexpr color4 green()       { return {0,1,0,1}; }
        static constexpr color4 blue()        { return {0,0,1,1}; }
        static constexpr color4 transparent() { return {0,0,0,0}; }
    };

    inline constexpr color3 color::toColor3() const { return color3(r, g, b); }
    inline constexpr color4 color::toColor4(f32 alpha) const { return color4(r, g, b, alpha); }
    inline constexpr color4 color3::toColor4(f32 alpha) const { return color4(r, g, b, alpha); }

    inline constexpr color3 operator*(f32 s, const color3& c) { return c * s; }
    inline constexpr color3 operator+(f32 s, const color3& c) { return c + s; }
    inline constexpr color4 operator*(f32 s, const color4& c) { return c * s; }
    inline constexpr color4 operator+(f32 s, const color4& c) { return c + s; }

    enum class LogLevel : u8 {
        Debug = 0, Info = 1, Warning = 2, Error = 3, Fatal = 4, None = 255
    };

    struct  LogEntry {
        LogLevel level;
        std::string message;
        std::string category;
        std::source_location location;
        std::chrono::system_clock::time_point timestamp;
        /** When VAPI_LOG_DETAIL: device name (hostname) and optional MAC identifier. */
        std::string deviceName;
        /** When VAPI_LOG_DETAIL: call-site execution address (e.g. from __builtin_return_address(0)). */
        std::uintptr_t callAddress{0};
    };

} // namespace vapi

#endif
/** @file platform_codes.hpp
 *  @brief Platform-independent key, mouse, and monitor types (Key, KeyMod, MouseButton, VideoMode, MonitorInfo, IMonitorProvider). */

#ifndef VAPI_PLATFORM_PLATFORM_CODES_HPP
#define VAPI_PLATFORM_PLATFORM_CODES_HPP

#include "core/types.hpp"
#include <string>
#include <vector>

namespace vapi {

// ─── Key codes ─────────────────────────────────────────────────────────────
enum class Key : s32 {
    Unknown = -1,
    Space = 32, Apostrophe = 39, Comma = 44, Minus = 45, Period = 46, Slash = 47,
    D0 = 48, D1, D2, D3, D4, D5, D6, D7, D8, D9,
    Semicolon = 59, Equal = 61,
    A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    LeftBracket = 91, Backslash = 92, RightBracket = 93, GraveAccent = 96,
    World1 = 161, World2 = 162,
    Escape = 256, Enter = 257, Tab = 258, Backspace = 259, Insert = 260, Delete = 261,
    Right = 262, Left = 263, Down = 264, Up = 265, PageUp = 266, PageDown = 267, Home = 268, End = 269,
    CapsLock = 280, ScrollLock = 281, NumLock = 282, PrintScreen = 283, Pause = 284,
    F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,
    KP0 = 320, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9,
    KPDecimal = 330, KPDivide = 331, KPMultiply = 332, KPSubtract = 333, KPAdd = 334, KPEnter = 335, KPEqual = 336,
    LeftShift = 340, LeftControl = 341, LeftAlt = 342, LeftSuper = 343,
    RightShift = 344, RightControl = 345, RightAlt = 346, RightSuper = 347, Menu = 348
};

enum class KeyAction : s32 { Release = 0, Press = 1, Repeat = 2 };

enum class KeyMod : s32 {
    None = 0, Shift = 0x0001, Control = 0x0002, Alt = 0x0004, Super = 0x0008, CapsLock = 0x0010, NumLock = 0x0020
};

inline KeyMod operator|(KeyMod a, KeyMod b) { return static_cast<KeyMod>(static_cast<s32>(a) | static_cast<s32>(b)); }
inline KeyMod operator&(KeyMod a, KeyMod b) { return static_cast<KeyMod>(static_cast<s32>(a) & static_cast<s32>(b)); }
inline bool hasFlag(KeyMod mods, KeyMod flag) { return (static_cast<s32>(mods) & static_cast<s32>(flag)) != 0; }

// ─── Mouse codes ───────────────────────────────────────────────────────────
enum class MouseButton : s32 { Left = 0, Right = 1, Middle = 2, B4 = 3, B5 = 4, B6 = 5, B7 = 6, B8 = 7 };

enum class CursorShape : s32 {
    Arrow = 0x00036001, IBeam = 0x00036002, Crosshair = 0x00036003,
    Hand = 0x00036004, HResize = 0x00036005, VResize = 0x00036006
};

// ─── Monitor ───────────────────────────────────────────────────────────────
struct VideoMode {
    u32 width{0};
    u32 height{0};
    u32 refreshRate{0};
    u32 redBits{8};
    u32 greenBits{8};
    u32 blueBits{8};
};

struct MonitorInfo {
    std::string name;
    s32 posX{0};
    s32 posY{0};
    u32 physicalWidth{0};
    u32 physicalHeight{0};
    VideoMode currentMode;
    std::vector<VideoMode> availableModes;
    f32 contentScaleX{1.f};
    f32 contentScaleY{1.f};
    void* nativeHandle{nullptr};
};

class IMonitorProvider {
public:
    virtual ~IMonitorProvider() = default;
    [[nodiscard]] virtual std::vector<MonitorInfo> getMonitors() const = 0;
    [[nodiscard]] virtual MonitorInfo getPrimaryMonitor() const = 0;
};

} // namespace vapi

#endif

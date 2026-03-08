#ifndef VAPI_CORE_I_WINDOW_HPP
#define VAPI_CORE_I_WINDOW_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <string>
#include <vector>
#include <string_view>
#include <functional>

namespace vapi {

enum class WindowMode : u8 {
    Windowed, Fullscreen, BorderlessFullscreen
};

/** How to handle window resize. */
enum class ResizeMode : u8 {
    /** Recalculate layout and positions of elements bound to the window size. */
    Stretch = 0,
    /** Show cached pre-rendered frames (with pixel shift) so image appears immediately when stretching, no bands. */
    RevealHidden = 1,
};

/** Resize behaviour: mode + reveal shift step (m = 1..3 pixels). */
struct ResizeConfig {
    ResizeMode mode{ResizeMode::Stretch};
    u8 revealShiftPixels{1};  // 1..3, used in RevealHidden: cache frames with shift 0, m, 2m, 3m
};

struct WindowConfig {
    std::string title{"VAPI Window"};
    u32 width{1280};
    u32 height{720};
    WindowMode mode{WindowMode::Windowed};
    bool resizable{true};
    bool decorated{true};
    bool focused{true};
    bool visible{true};
    s32 posX{-1};
    s32 posY{-1};
    ResizeConfig resize{};
};

struct WindowState {
    u32 width{0};
    u32 height{0};
    u32 framebufferWidth{0};
    u32 framebufferHeight{0};
    s32 posX{0};
    s32 posY{0};
    bool minimized{false};
    bool maximized{false};
    bool focused{false};
    bool shouldClose{false};
};

struct WindowCallbacks {
    std::function<void(WindowId, u32, u32)>  onResize;
    std::function<void(WindowId, u32, u32)>  onFramebufferResize;
    std::function<void(WindowId, s32, s32)>   onMove;
    std::function<void(WindowId, bool)>      onFocus;
    std::function<void(WindowId, bool)>      onMinimize;
    std::function<void(WindowId, bool)>      onMaximize;
    std::function<void(WindowId)>            onClose;
};

/** Single interface for all window control and query operations (visibility, size, position, focus, title, close, state). */
class IWindowControl {
public:
    virtual ~IWindowControl() = default;
    virtual void showWindow(WindowId id) = 0;
    virtual void hideWindow(WindowId id) = 0;
    virtual void setWindowSize(WindowId id, u32 width, u32 height) = 0;
    virtual void setWindowMode(WindowId id, WindowMode mode) = 0;
    virtual void setWindowPos(WindowId id, s32 x, s32 y) = 0;
    virtual void focusWindow(WindowId id) = 0;
    virtual void minimizeWindow(WindowId id) = 0;
    virtual void maximizeWindow(WindowId id) = 0;
    virtual void restoreWindow(WindowId id) = 0;
    virtual void setWindowTitle(WindowId id, std::string_view title) = 0;
    virtual void requestClose(WindowId id) = 0;
    virtual void cancelClose(WindowId id) = 0;
    [[nodiscard]] virtual bool shouldClose(WindowId id) const = 0;
    [[nodiscard]] virtual WindowState getWindowState(WindowId id) const = 0;
    [[nodiscard]] virtual bool windowExists(WindowId id) const = 0;
    [[nodiscard]] virtual std::vector<WindowId> getAllWindowIds() const = 0;
    /** Current resize mode and reveal shift (m). */
    [[nodiscard]] virtual ResizeConfig getResizeConfig(WindowId id) const = 0;
    virtual void setResizeConfig(WindowId id, ResizeConfig config) = 0;
};

/** Full window interface: lifecycle (create/destroy), callbacks, native handle, and IWindowControl. */
class IWindow : public IWindowControl {
public:
    virtual ~IWindow() = default;
    [[nodiscard]] virtual Result<WindowId> createWindow(const WindowConfig& config) = 0;
    [[nodiscard]] virtual Result<void> destroyWindow(WindowId id) = 0;
    virtual void destroyAllWindows() = 0;
    virtual void setCallbacks(WindowId id, const WindowCallbacks& callbacks) = 0;
    virtual void setGlobalCallbacks(const WindowCallbacks& callbacks) = 0;
    [[nodiscard]] virtual void* getNativeHandle(WindowId id) const = 0;
};

} // namespace vapi

#endif

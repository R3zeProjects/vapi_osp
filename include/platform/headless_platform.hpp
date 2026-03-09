/** @file headless_platform.hpp
 *  @brief Headless platform backend: no real window, for CI and tests without display. */

#ifndef VAPI_PLATFORM_HEADLESS_PLATFORM_HPP
#define VAPI_PLATFORM_HEADLESS_PLATFORM_HPP

#include "platform/i_platform.hpp"
#include "platform/platform_codes.hpp"
#include "core/error.hpp"
#include <chrono>
#include <vector>

namespace vapi {

/** Platform backend that never creates a real window. anyWindowOpen() is always false.
 *  Use for headless tests and CI. createWindow() returns NotSupported. */
class HeadlessPlatform : public IPlatform {
public:
    HeadlessPlatform() = default;
    ~HeadlessPlatform() override = default;

    Result<void> init() override;
    bool isInitialized() const override { return m_initialized; }
    void terminate() override;

    std::vector<const char*> getRequiredVulkanExtensions() const override;
    double getTimeSeconds() const override;

    Result<WindowId> createWindow(const WindowConfig&) override;
    Result<void> destroyWindow(WindowId) override;
    void destroyAllWindows() override;
    void setCallbacks(WindowId, const WindowCallbacks&) override;
    void setGlobalCallbacks(const WindowCallbacks&) override;
    void* getNativeHandle(WindowId) const override;

    void showWindow(WindowId) override {}
    void hideWindow(WindowId) override {}
    void setWindowSize(WindowId, u32, u32) override {}
    void setWindowMode(WindowId, WindowMode) override {}
    void setWindowPos(WindowId, s32, s32) override {}
    void focusWindow(WindowId) override {}
    void minimizeWindow(WindowId) override {}
    void maximizeWindow(WindowId) override {}
    void restoreWindow(WindowId) override {}
    void setWindowTitle(WindowId, std::string_view) override {}
    void requestClose(WindowId) override {}
    void cancelClose(WindowId) override {}
    bool shouldClose(WindowId) const override { return true; }
    WindowState getWindowState(WindowId) const override;
    bool windowExists(WindowId) const override { return false; }
    std::vector<WindowId> getAllWindowIds() const override { return {}; }
    ResizeConfig getResizeConfig(WindowId) const override { return {}; }
    void setResizeConfig(WindowId, ResizeConfig) override {}

    void setInputCallbacks(WindowId, const InputCallbacks&) override {}
    void setGlobalInputCallbacks(const InputCallbacks&) override {}
    bool isKeyPressed(WindowId, s32) const override { return false; }
    bool isKeyReleased(WindowId, s32) const override { return true; }
    s32 getKeyState(WindowId, s32) const override { return 0; }
    bool isButtonPressed(WindowId, s32) const override { return false; }
    bool isButtonReleased(WindowId, s32) const override { return true; }
    std::pair<f64, f64> getCursorPos(WindowId) const override { return {0, 0}; }
    std::pair<f32, f32> getCursorPosInFramebuffer(WindowId) const override { return {0.f, 0.f}; }
    void setCursorPos(WindowId, f64, f64) override {}
    void setCursorMode(WindowId, CursorMode) override {}
    bool isGamepadConnected(s32) const override { return false; }
    const char* getGamepadName(s32) const override { return ""; }
    bool getGamepadState(s32, void*) const override { return false; }
    void pollEvents() override {}
    void waitEvents() override {}
    void waitEventsTimeout(f64) override {}
    void postEmptyEvent() override {}
    bool anyWindowOpen() const override { return false; }

    std::vector<MonitorInfo> getMonitors() const override { return {}; }
    MonitorInfo getPrimaryMonitor() const override { return {}; }

private:
    bool m_initialized{false};
    std::chrono::steady_clock::time_point m_start;
};

} // namespace vapi

#endif // VAPI_PLATFORM_HEADLESS_PLATFORM_HPP

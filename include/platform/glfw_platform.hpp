/** @file glfw_platform.hpp
 *  @brief GLFW backend: GlfwInit (RAII), glfw_utils (hints, monitor, Vulkan extensions), GlfwPlatform (IWindow + IInput + IMonitorProvider). */

#ifndef VAPI_PLATFORM_GLFW_PLATFORM_HPP
#define VAPI_PLATFORM_GLFW_PLATFORM_HPP

#include "core/error.hpp"
#include "core/logger.hpp"
#include "platform/i_platform.hpp"
#include "platform/platform_codes.hpp"
#include <GLFW/glfw3.h>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <string>

namespace vapi {

// ─── GlfwInit: RAII init/terminate ─────────────────────────────────────────
class GlfwInit {
public:
    [[nodiscard]] static Result<void> ensure() {
        bool expected = false;
        if (s_initialized.compare_exchange_strong(expected, true)) {
            glfwSetErrorCallback([](int code, const char* desc) {
                logError("GLFW", std::string("Error ") + std::to_string(code) + ": " + desc);
            });
            if (!glfwInit()) {
                s_initialized = false;
                return std::unexpected(errors::Init);
            }
            logInfo("GLFW", "Initialized");
        }
        return {};
    }
    static void terminate() {
        bool expected = true;
        if (s_initialized.compare_exchange_strong(expected, false)) {
            glfwTerminate();
            logInfo("GLFW", "Terminated");
        }
    }
    [[nodiscard]] static bool isInitialized() { return s_initialized; }
private:
    static inline std::atomic<bool> s_initialized{false};
};

// ─── glfw_utils: VAPI ↔ GLFW ───────────────────────────────────────────────
namespace glfw_utils {
    inline void applyHints(const WindowConfig& cfg) {
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, cfg.resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, cfg.decorated ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUSED, cfg.focused ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, cfg.visible ? GLFW_TRUE : GLFW_FALSE);
    }
    inline MonitorInfo toMonitorInfo(GLFWmonitor* mon) {
        MonitorInfo info;
        info.nativeHandle = mon;
        if (const char* name = glfwGetMonitorName(mon)) info.name = name;
        glfwGetMonitorPos(mon, &info.posX, &info.posY);
        int pw, ph;
        glfwGetMonitorPhysicalSize(mon, &pw, &ph);
        info.physicalWidth = static_cast<u32>(pw);
        info.physicalHeight = static_cast<u32>(ph);
        float sx, sy;
        glfwGetMonitorContentScale(mon, &sx, &sy);
        info.contentScaleX = sx;
        info.contentScaleY = sy;
        if (const GLFWvidmode* vm = glfwGetVideoMode(mon)) {
            info.currentMode = { static_cast<u32>(vm->width), static_cast<u32>(vm->height),
                static_cast<u32>(vm->refreshRate), static_cast<u32>(vm->redBits),
                static_cast<u32>(vm->greenBits), static_cast<u32>(vm->blueBits) };
        }
        int count;
        if (const GLFWvidmode* modes = glfwGetVideoModes(mon, &count)) {
            info.availableModes.reserve(count);
            for (int i = 0; i < count; ++i) {
                info.availableModes.push_back({ static_cast<u32>(modes[i].width), static_cast<u32>(modes[i].height),
                    static_cast<u32>(modes[i].refreshRate), static_cast<u32>(modes[i].redBits),
                    static_cast<u32>(modes[i].greenBits), static_cast<u32>(modes[i].blueBits) });
            }
        }
        return info;
    }
    inline std::vector<const char*> getRequiredVulkanExtensions() {
        uint32_t count = 0;
        const char** exts = glfwGetRequiredInstanceExtensions(&count);
        if (!exts) return {};
        return { exts, exts + count };
    }
}

// ─── GlfwPlatform ───────────────────────────────────────────────────────────
class GlfwPlatform final : public IPlatform {
public:
    GlfwPlatform();
    ~GlfwPlatform() override;
    GlfwPlatform(const GlfwPlatform&) = delete;
    GlfwPlatform& operator=(const GlfwPlatform&) = delete;

    Result<void> init() override;
    [[nodiscard]] bool isInitialized() const override;
    void terminate() override;

    [[nodiscard]] Result<WindowId> createWindow(const WindowConfig& config) override;
    [[nodiscard]] Result<void> destroyWindow(WindowId id) override;
    void destroyAllWindows() override;
    void showWindow(WindowId id) override;
    void hideWindow(WindowId id) override;
    void setWindowSize(WindowId id, u32 w, u32 h) override;
    void setWindowMode(WindowId id, WindowMode mode) override;
    void setWindowPos(WindowId id, s32 x, s32 y) override;
    void focusWindow(WindowId id) override;
    void minimizeWindow(WindowId id) override;
    void maximizeWindow(WindowId id) override;
    void restoreWindow(WindowId id) override;
    void setWindowTitle(WindowId id, std::string_view title) override;
    void requestClose(WindowId id) override;
    void cancelClose(WindowId id) override;
    [[nodiscard]] bool shouldClose(WindowId id) const override;
    [[nodiscard]] WindowState getWindowState(WindowId id) const override;
    [[nodiscard]] bool windowExists(WindowId id) const override;
    [[nodiscard]] std::vector<WindowId> getAllWindowIds() const override;
    [[nodiscard]] ResizeConfig getResizeConfig(WindowId id) const override;
    void setResizeConfig(WindowId id, ResizeConfig config) override;
    void setCallbacks(WindowId id, const WindowCallbacks& cb) override;
    void setGlobalCallbacks(const WindowCallbacks& cb) override;
    [[nodiscard]] void* getNativeHandle(WindowId id) const override;

    [[nodiscard]] bool isKeyPressed(WindowId id, s32 key) const override;
    [[nodiscard]] bool isKeyReleased(WindowId id, s32 key) const override;
    [[nodiscard]] s32 getKeyState(WindowId id, s32 key) const override;
    [[nodiscard]] bool isButtonPressed(WindowId id, s32 button) const override;
    [[nodiscard]] bool isButtonReleased(WindowId id, s32 button) const override;
    [[nodiscard]] std::pair<f64, f64> getCursorPos(WindowId id) const override;
    [[nodiscard]] std::pair<f32, f32> getCursorPosInFramebuffer(WindowId id) const override;
    void setCursorPos(WindowId id, f64 x, f64 y) override;
    void setCursorMode(WindowId id, CursorMode mode) override;
    [[nodiscard]] bool isGamepadConnected(s32 id) const override;
    [[nodiscard]] const char* getGamepadName(s32 id) const override;
    [[nodiscard]] bool getGamepadState(s32 id, void* state) const override;
    void pollEvents() override;
    void waitEvents() override;
    void waitEventsTimeout(f64 timeout) override;
    void postEmptyEvent() override;
    [[nodiscard]] bool anyWindowOpen() const override;
    void setInputCallbacks(WindowId id, const InputCallbacks& cb) override;
    void setGlobalInputCallbacks(const InputCallbacks& cb) override;

    [[nodiscard]] std::vector<MonitorInfo> getMonitors() const override;
    [[nodiscard]] MonitorInfo getPrimaryMonitor() const override;

    [[nodiscard]] GLFWwindow* getGlfwWindow(WindowId id) const;
    [[nodiscard]] std::vector<const char*> getRequiredVulkanExtensions() const override;
    [[nodiscard]] double getTimeSeconds() const override;

private:
    struct WindowEntry {
        GLFWwindow* handle{nullptr};
        WindowConfig config;
        WindowCallbacks windowCallbacks;
        InputCallbacks inputCallbacks;
        WindowMode currentMode{WindowMode::Windowed};
        s32 savedX{0}, savedY{0};
        u32 savedW{0}, savedH{0};
    };
    WindowEntry* findEntry(WindowId id);
    const WindowEntry* findEntry(WindowId id) const;
    GLFWwindow* findHandle(WindowId id) const;
    void installCallbacks(WindowId id, GLFWwindow* win);

    static void keyCallback(GLFWwindow*, int, int, int, int);
    static void charCallback(GLFWwindow*, unsigned int);
    static void mouseButtonCallback(GLFWwindow*, int, int, int);
    static void cursorPosCallback(GLFWwindow*, double, double);
    static void cursorEnterCallback(GLFWwindow*, int);
    static void scrollCallback(GLFWwindow*, double, double);
    static void dropCallback(GLFWwindow*, int, const char**);
    static void windowSizeCallback(GLFWwindow*, int, int);
    static void framebufferSizeCallback(GLFWwindow*, int, int);
    static void windowPosCallback(GLFWwindow*, int, int);
    static void windowFocusCallback(GLFWwindow*, int);
    static void windowIconifyCallback(GLFWwindow*, int);
    static void windowMaximizeCallback(GLFWwindow*, int);
    static void windowCloseCallback(GLFWwindow*);

    std::unordered_map<WindowId, WindowEntry> m_windows;
    std::unordered_map<GLFWwindow*, WindowId> m_handleToId;
    WindowCallbacks m_globalWindowCb;
    InputCallbacks m_globalInputCb;
    WindowId m_nextId{1};
    bool m_initialized{false};
    mutable std::mutex m_mutex;
};

} // namespace vapi

#endif

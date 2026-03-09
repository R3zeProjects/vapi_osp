# Platform module — Detailed API

Include: `#include "platform/platform.hpp"` (brings in platform_codes, i_platform, glfw_platform, createPlatform). Or use root `#include "vapi.hpp"`.

---

## 1. createPlatform()

```cpp
std::unique_ptr<vapi::IPlatform> platform = vapi::createPlatform();
```

Returns the only currently supported backend: **GlfwPlatform** (GLFW). Use **IPlatform** in types and documentation; the alias **IPlatformBackend** exists for backward compatibility.

---

## 2. IPlatform

**Interface:** Combines `IWindow`, `IInput`, `IMonitorProvider`, `IInitializable`, plus Vulkan extensions.

- **Lifecycle:** `init()` → `Result<void>`, `isInitialized()`, `terminate()`.
- **Vulkan:** `getRequiredVulkanExtensions()` → `std::vector<const char*>` (pass into `InstanceConfig::requiredExtensions`).
- All window methods come from **IWindow** (see IWindowControl + IWindow below).
- All input and event methods from **IInput** (keyboard, mouse, gamepad, poll, callbacks).

```cpp
if (!platform->init().has_value()) return 1;
auto exts = platform->getRequiredVulkanExtensions();
// use exts for Vulkan instance creation
```

---

## 3. IWindowControl and IWindow

### IWindowControl

Single interface for all window control and query:

- **Visibility:** `showWindow(id)`, `hideWindow(id)`.
- **Size/mode:** `setWindowSize(id, width, height)`, `setWindowMode(id, WindowMode)`.
- **Position:** `setWindowPos(id, x, y)`.
- **Focus/state:** `focusWindow(id)`, `minimizeWindow(id)`, `maximizeWindow(id)`, `restoreWindow(id)`.
- **Title:** `setWindowTitle(id, title)`.
- **Close:** `requestClose(id)`, `cancelClose(id)`, `shouldClose(id)`.
- **Query:** `getWindowState(id)`, `windowExists(id)`, `getAllWindowIds()`, `getResizeConfig(id)`, `setResizeConfig(id, ResizeConfig)`.

### IWindow (extends IWindowControl)

Adds lifecycle and callbacks:

- **Lifecycle:** `createWindow(config)` → `Result<WindowId>`, `destroyWindow(id)`, `destroyAllWindows()`.
- **Callbacks:** `setCallbacks(id, WindowCallbacks)`, `setGlobalCallbacks(WindowCallbacks)`.
- **Native handle:** `getNativeHandle(id)` → `void*` (e.g. `GLFWwindow*` for surface creation).

---

## 4. WindowConfig and WindowState

### WindowConfig

- **Fields:** `title`, `width`, `height`, `mode` (Windowed / Fullscreen / BorderlessFullscreen), `resizable`, `decorated`, `focused`, `visible`, `posX`, `posY`, **`resize`** (ResizeConfig).
- **Default:** 1280×720, windowed, resizable.

### ResizeMode and ResizeConfig

- **ResizeMode::Stretch** — on resize, element positions bound to window size are recalculated (layout).
- **ResizeMode::RevealHidden** — a cache of pre-processed frames (shifted by 0, m, 2m, 3m pixels) is displayed so that stretching the window shows an image immediately instead of blank bars.
- **ResizeConfig:** `mode`, `revealShiftPixels` (m = 1..3). Set via `WindowConfig.resize` or through `setResizeConfig(id, config)` / `getResizeConfig(id)`.
- In **Stretch** mode, the application recalculates layout and redraws in `onFramebufferResize`. In **RevealHidden** mode, the application uses **ResizeRevealCache** (`core/tools/resize_reveal_cache.hpp`): each frame calls `pushFrame(pixels, w, h)`, and on resize it draws a cached frame via `frame(variantForSize(newW, newH))` until the next full redraw.

### WindowState

- **Fields:** `width`, `height`, `framebufferWidth`, `framebufferHeight`, `posX`, `posY`, `minimized`, `maximized`, `focused`, `shouldClose`.
- **Use:** Returned by `getWindowState(id)`; e.g. use `framebufferWidth`/`framebufferHeight` for Vulkan swapchain/resize.

### WindowCallbacks

- **Fields:** `onResize`, `onFramebufferResize`, `onMove`, `onFocus`, `onMinimize`, `onMaximize`, `onClose` (all take `WindowId` and relevant args).
- **Use:** `setCallbacks(winId, wcb)` or `setGlobalCallbacks(wcb)`.

```cpp
vapi::WindowConfig cfg;
cfg.title = "My Window";
cfg.width = 1920;
cfg.height = 1080;
auto wid = platform->createWindow(cfg);
if (!wid) return;
vapi::WindowId winId = *wid;
vapi::WindowCallbacks wcb;
wcb.onFramebufferResize = [](vapi::WindowId, u32 w, u32 h) { /* resize swapchain to w,h */ };
platform->setCallbacks(winId, wcb);
void* native = platform->getNativeHandle(winId);  // e.g. for VkSurfaceWrapper
```

---

## 5. Input (platform_codes.hpp, i_input.hpp)

### InputCallbacks

- **Fields:** `onKey(WindowId, key, scancode, action, mods)`, `onChar(WindowId, codepoint)`, `onMouseButton(WindowId, button, action, mods)`, `onCursorPos(WindowId, x, y)`, `onCursorEnter(WindowId, entered)`, `onScroll(WindowId, x, y)`, `onFileDrop(WindowId, paths)`.
- **Use:** `setInputCallbacks(winId, icb)` or `setGlobalInputCallbacks(icb)`.

### Key and button enums (platform_codes.hpp)

- **Key:** `Key::Escape`, `Key::Space`, `Key::A`…`Key::Z`, arrows, F1–F25, keypad, modifiers (LeftShift, etc.).
- **KeyAction:** `Release`, `Press`, `Repeat`.
- **KeyMod:** `None`, `Shift`, `Control`, `Alt`, `Super`, `CapsLock`, `NumLock`; composable with `|`, `&`; `hasFlag(mods, flag)`.
- **MouseButton:** `Left`, `Right`, `Middle`, `B4`–`B8`.
- **CursorMode:** `Normal`, `Hidden`, `Disabled` (for input interface).

### Polling and events

- **IPollable (via IInput):** `pollEvents()`, `waitEvents()`, `waitEventsTimeout(timeout)`, `postEmptyEvent()`, `anyWindowOpen()`.
- **Keyboard:** `isKeyPressed(id, key)`, `isKeyReleased(id, key)`, `getKeyState(id, key)`.
- **Mouse:** `isButtonPressed`/`Released`, `getCursorPos`, `getCursorPosInFramebuffer`, `setCursorPos`, `setCursorMode`.
- **Gamepad:** `isGamepadConnected(id)`, `getGamepadName(id)`, `getGamepadState(id, state)`.

```cpp
vapi::InputCallbacks icb;
icb.onKey = [&](vapi::WindowId id, s32 key, s32, s32 action, s32) {
    if (key == static_cast<s32>(vapi::Key::Escape) && action == static_cast<s32>(vapi::KeyAction::Press))
        platform->requestClose(id);
};
platform->setInputCallbacks(winId, icb);
while (platform->anyWindowOpen())
    platform->pollEvents();
```

---

## 6. Monitors (IMonitorProvider)

- **getMonitors()** → `std::vector<MonitorInfo>`.
- **getPrimaryMonitor()** → `MonitorInfo`.
- **MonitorInfo:** `name`, `posX`, `posY`, `physicalWidth`/`Height`, `currentMode` (VideoMode), `availableModes`, `contentScaleX`/`Y`, `nativeHandle`.
- **VideoMode:** `width`, `height`, `refreshRate`, `redBits`, `greenBits`, `blueBits`.

```cpp
auto monitors = platform->getMonitors();
for (const auto& m : monitors)
    vapi::logInfo("Platform", std::string("Monitor: ") + m.name + " " + std::to_string(m.currentMode.width) + "x" + std::to_string(m.currentMode.height));

vapi::MonitorInfo primary = platform->getPrimaryMonitor();
u32 fbWidth = primary.currentMode.width;
u32 fbHeight = primary.currentMode.height;
```

---

## 7. GLFW-specific (glfw_platform.hpp)

- **GlfwInit::ensure()** → `Result<void>` (init GLFW once); **GlfwInit::terminate()**; **GlfwInit::isInitialized()**.
- **glfw_utils::applyHints(WindowConfig)** — sets GLFW window hints from config.
- **glfw_utils::toMonitorInfo(GLFWmonitor*)** — builds `MonitorInfo`.
- **glfw_utils::getRequiredVulkanExtensions()** — returns required instance extensions.
- **GlfwPlatform::getGlfwWindow(WindowId)** — returns `GLFWwindow*` for that window (if you need it directly).

```cpp
// Optional: ensure GLFW is initialized before createPlatform() if needed
vapi::GlfwInit::ensure();

// Get GLFW window pointer for Vulkan surface (when not using GpuContext)
vapi::GlfwPlatform* glfw = static_cast<vapi::GlfwPlatform*>(platform.get());
GLFWwindow* win = glfw->getGlfwWindow(winId);
```

Typically you only use `createPlatform()` and `IPlatform`; the rest is implementation detail unless you need GLFW-specific APIs.

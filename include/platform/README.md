# Platform module

Window, input, and monitor abstraction with GLFW backend. Single backend (GLFW); structure allows future Win32/SDL.

- **platform_codes.hpp** — Key, KeyAction, KeyMod, MouseButton, CursorShape, VideoMode, MonitorInfo, IMonitorProvider.
- **i_platform.hpp** — IPlatform (IWindow + IInput + IMonitorProvider + IInitializable + getRequiredVulkanExtensions).
- **glfw_platform.hpp** — GlfwInit (RAII), glfw_utils (applyHints, toMonitorInfo, getRequiredVulkanExtensions), GlfwPlatform.
- **glfw_platform.cpp** — GlfwPlatform implementation.
- **platform.hpp** — Master include; createPlatform().

Usage: `#include "platform/platform.hpp"`. Link: `vapi_platform` (glfw, vapi_core). Interaction with GPU: pass `getRequiredVulkanExtensions()` into `InstanceConfig::requiredExtensions`; use `getNativeHandle(WindowId)` for `VkSurfaceWrapper::create(instance, (GLFWwindow*)handle)`.

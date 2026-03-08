#ifndef VAPI_CORE_I_INPUT_HPP
#define VAPI_CORE_I_INPUT_HPP

#include "core/types.hpp"
#include <vector>
#include <string>
#include <functional>
#include <utility>

namespace vapi {

enum class CursorMode : u8 { Normal, Hidden, Disabled };

struct InputCallbacks {
    std::function<void(WindowId, s32, s32, s32, s32)> onKey;
    std::function<void(WindowId, u32)> onChar;
    std::function<void(WindowId, s32, s32, s32)> onMouseButton;
    std::function<void(WindowId, f64, f64)> onCursorPos;
    std::function<void(WindowId, bool)> onCursorEnter;
    std::function<void(WindowId, f64, f64)> onScroll;
    std::function<void(WindowId, std::vector<std::string>)> onFileDrop;
};

class IKeyboardInput {
public:
    virtual ~IKeyboardInput() = default;
    [[nodiscard]] virtual bool isKeyPressed(WindowId id, s32 key) const = 0;
    [[nodiscard]] virtual bool isKeyReleased(WindowId id, s32 key) const = 0;
    [[nodiscard]] virtual s32 getKeyState(WindowId id, s32 key) const = 0;
};

class IMouseInput {
public:
    virtual ~IMouseInput() = default;
    [[nodiscard]] virtual bool isButtonPressed(WindowId id, s32 button) const = 0;
    [[nodiscard]] virtual bool isButtonReleased(WindowId id, s32 button) const = 0;
    [[nodiscard]] virtual std::pair<f64, f64> getCursorPos(WindowId id) const = 0;
    [[nodiscard]] virtual std::pair<f32, f32> getCursorPosInFramebuffer(WindowId id) const = 0;
    virtual void setCursorPos(WindowId id, f64 x, f64 y) = 0;
    virtual void setCursorMode(WindowId id, CursorMode mode) = 0;
};

class IGamepadInput {
public:
    virtual ~IGamepadInput() = default;
    [[nodiscard]] virtual bool isGamepadConnected(s32 id) const = 0;
    [[nodiscard]] virtual const char* getGamepadName(s32 id) const = 0;
    [[nodiscard]] virtual bool getGamepadState(s32 id, void* state) const = 0;
};

class IPollable {
public:
    virtual ~IPollable() = default;
    virtual void pollEvents() = 0;
    virtual void waitEvents() = 0;
    virtual void waitEventsTimeout(f64 timeout) = 0;
    virtual void postEmptyEvent() = 0;
    [[nodiscard]] virtual bool anyWindowOpen() const = 0;
};

class IInput : public IKeyboardInput, public IMouseInput, public IGamepadInput, public IPollable {
public:
    virtual ~IInput() = default;
    virtual void setInputCallbacks(WindowId id, const InputCallbacks& cb) = 0;
    virtual void setGlobalInputCallbacks(const InputCallbacks& cb) = 0;
};

} // namespace vapi

#endif

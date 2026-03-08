#ifndef VAPI_CORE_I_DRAWABLE_HPP
#define VAPI_CORE_I_DRAWABLE_HPP

#include "core/types.hpp"
#include "core/error.hpp"

namespace vapi {

struct DrawContext {
    rect   viewport{};
    f32    deltaTime{0.f};
    u32    frameIndex{0};
    void*  userData{nullptr};
};

class IDrawable {
public:
    virtual ~IDrawable() = default;
    virtual void draw(const DrawContext& ctx) = 0;
    [[nodiscard]] virtual bool isVisible() const { return true; }
};

class IRenderTarget {
public:
    virtual ~IRenderTarget() = default;
    [[nodiscard]] virtual u32 width()  const = 0;
    [[nodiscard]] virtual u32 height() const = 0;
    [[nodiscard]] virtual Result<void> beginRender() = 0;
    [[nodiscard]] virtual Result<void> endRender() = 0;
};

class ILayered {
public:
    virtual ~ILayered() = default;
    virtual void setZOrder(s32 z) = 0;
    [[nodiscard]] virtual s32 zOrder() const = 0;
};

} // namespace vapi

#endif

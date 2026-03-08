#ifndef VAPI_CORE_I_UPDATABLE_HPP
#define VAPI_CORE_I_UPDATABLE_HPP

#include "core/types.hpp"

namespace vapi {

class IUpdatable {
public:
    virtual ~IUpdatable() = default;
    virtual void update(f64 dt) = 0;
};
class IFixedTick {
public:
    virtual ~IFixedTick() = default;
    virtual void fixedUpdate(f64 fixedDt) = 0;
};
class IDirtyFlag {
public:
    virtual ~IDirtyFlag() = default;
    [[nodiscard]] virtual bool isDirty() const = 0;
    virtual void markDirty() = 0;
    virtual void clearDirty() = 0;
};

} // namespace vapi

#endif

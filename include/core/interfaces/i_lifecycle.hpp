#ifndef VAPI_CORE_I_LIFECYCLE_HPP
#define VAPI_CORE_I_LIFECYCLE_HPP

#include "core/types.hpp"
#include "core/error.hpp"

namespace vapi {

template<typename Handle, typename Desc>
class ICreatable {
public:
    virtual ~ICreatable() = default;
    [[nodiscard]] virtual Result<Handle> create(const Desc& desc) = 0;
};

template<typename Handle>
class IDestroyable {
public:
    virtual ~IDestroyable() = default;
    [[nodiscard]] virtual Result<void> destroy(Handle handle) = 0;
    virtual void destroyAll() = 0;
};

template<typename Handle>
class IResetable {
public:
    virtual ~IResetable() = default;
    [[nodiscard]] virtual Result<void> reset(Handle handle, u32 flags = 0) = 0;
};

template<typename Handle, typename Desc>
class IRecreatable {
public:
    virtual ~IRecreatable() = default;
    [[nodiscard]] virtual Result<Handle> recreate(Handle handle, const Desc& desc) = 0;
};

template<typename Handle>
class IValidatable {
public:
    virtual ~IValidatable() = default;
    [[nodiscard]] virtual bool isValid(Handle handle) const = 0;
};

class IInitializable {
public:
    virtual ~IInitializable() = default;
    virtual Result<void> init() = 0;
    virtual bool isInitialized() const = 0;
    virtual void terminate() = 0;
};

} // namespace vapi

#endif

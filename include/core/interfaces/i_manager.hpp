#ifndef VAPI_CORE_I_MANAGER_HPP
#define VAPI_CORE_I_MANAGER_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <utility>

namespace vapi {

template<typename Key, typename Derived, typename... Prms>
class IManager {
public:
    virtual ~IManager() = default;
    template<typename... Args>
    Result<u32> create(Args&&... args) {
        return static_cast<Derived*>(this)->touch(
            static_cast<Prms>(std::forward<Args>(args))...);
    }
    virtual Result<u32> rm(Key key) = 0;
};

} // namespace vapi

#endif

#ifndef VAPI_CORE_I_EVENT_HPP
#define VAPI_CORE_I_EVENT_HPP

#include "core/types.hpp"
#include <any>
#include <string_view>
#include <functional>

namespace vapi {

struct Event {
    u32             type{0};
    std::any        payload;
    std::string_view name;
};

using EventCallback = std::function<void(const Event&)>;

class IEventEmitter {
public:
    virtual ~IEventEmitter() = default;
    virtual u32  subscribe(u32 eventType, EventCallback cb) = 0;
    virtual void unsubscribe(u32 subscriptionId) = 0;
    virtual void emit(const Event& event) = 0;
};

class IEventListener {
public:
    virtual ~IEventListener() = default;
    virtual void onEvent(const Event& event) = 0;
};

} // namespace vapi

#endif

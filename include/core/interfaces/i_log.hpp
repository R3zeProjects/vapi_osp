#ifndef VAPI_CORE_I_LOG_HPP
#define VAPI_CORE_I_LOG_HPP

#include "core/types.hpp"

namespace vapi {

class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
};

} // namespace vapi

#endif

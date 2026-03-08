/** @file i_platform.hpp
 *  @brief Combined platform interface: IWindow + IInput + IMonitorProvider + IInitializable + getRequiredVulkanExtensions. */

#ifndef VAPI_PLATFORM_I_PLATFORM_HPP
#define VAPI_PLATFORM_I_PLATFORM_HPP

#include "core/interfaces/i_window.hpp"
#include "core/interfaces/i_input.hpp"
#include "core/interfaces/i_lifecycle.hpp"
#include "platform/platform_codes.hpp"
#include <vector>

namespace vapi {

struct IPlatform : public IWindow, public IInput, public IMonitorProvider, public IInitializable {
    virtual ~IPlatform() = default;
    [[nodiscard]] virtual std::vector<const char*> getRequiredVulkanExtensions() const = 0;
    /// Время в секундах с момента инициализации платформы (для анимаций, таймеров).
    [[nodiscard]] virtual double getTimeSeconds() const = 0;
};

} // namespace vapi

#endif

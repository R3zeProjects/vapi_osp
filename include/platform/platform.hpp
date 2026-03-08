/** @file platform.hpp
 *  @brief Platform backend interface (IPlatform) and factory. No GLFW dependency here; include glfw_platform.hpp only when you need the GLFW implementation. */

#ifndef VAPI_PLATFORM_PLATFORM_HPP
#define VAPI_PLATFORM_PLATFORM_HPP

#include "platform/platform_codes.hpp"
#include "platform/i_platform.hpp"
#include <memory>

namespace vapi {

/** Parent type for all platform backends. Use createPlatform() or createPlatformShared() to get the implementation. */
using IPlatformBackend = IPlatform;

/** Available platform backends. Add new value and case in platform_factory.cpp to plug another implementation. */
enum class PlatformKind : unsigned {
    Glfw = 0,
};

/** Create platform backend (single owner). kind selects implementation; default Glfw. Link vapi_platform for Glfw. */
[[nodiscard]] std::unique_ptr<IPlatform> createPlatform(PlatformKind kind = PlatformKind::Glfw);

/** Create platform backend (shared ownership). Use when several components share the same platform instance. */
[[nodiscard]] std::shared_ptr<IPlatform> createPlatformShared(PlatformKind kind = PlatformKind::Glfw);

} // namespace vapi

#endif

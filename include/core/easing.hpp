#ifndef VAPI_CORE_EASING_HPP
#define VAPI_CORE_EASING_HPP

/** @file easing.hpp
 *  @brief Easing enum and applyEasing(t) for animation. */

#include "core/types.hpp"

namespace vapi {

[[nodiscard]] f32 applyEasing(Easing e, f32 t);

} // namespace vapi

#endif

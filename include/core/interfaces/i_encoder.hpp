/** @file i_encoder.hpp
 *  @deprecated Moved to core/interfaces/future/i_encoder.hpp (namespace vapi::future).
 *  This header is kept for source compatibility but IEncoder has no shipping implementation.
 *  Include future/i_encoder.hpp directly if you plan to implement this interface. */

#ifndef VAPI_CORE_I_ENCODER_HPP
#define VAPI_CORE_I_ENCODER_HPP

#include "core/interfaces/future/i_encoder.hpp"

namespace vapi {
    using IEncoder = vapi::future::IEncoder;
} // namespace vapi

#endif // VAPI_CORE_I_ENCODER_HPP

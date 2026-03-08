/** @file future/i_encoder.hpp
 *  @brief Reserved interface for data encoding/decoding — no implementation yet.
 *  @note This interface is intentionally not part of the public vapi.hpp include.
 *        It is provided for future backend implementors. Signatures may change. */

#ifndef VAPI_CORE_FUTURE_I_ENCODER_HPP
#define VAPI_CORE_FUTURE_I_ENCODER_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <span>
#include <vector>

namespace vapi::future {

class IEncoder {
public:
    virtual ~IEncoder() = default;
    [[nodiscard]] virtual Result<std::vector<u8>> encode(std::span<const u8> src) = 0;
    [[nodiscard]] virtual Result<std::vector<u8>> decode(std::span<const u8> src) = 0;
};

} // namespace vapi::future

#endif // VAPI_CORE_FUTURE_I_ENCODER_HPP

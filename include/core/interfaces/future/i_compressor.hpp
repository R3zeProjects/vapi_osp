/** @file future/i_compressor.hpp
 *  @brief Reserved interface for data compression — no implementation yet.
 *  @note This interface is intentionally not part of the public vapi.hpp include.
 *        It is provided for future backend implementors. Signatures may change. */

#ifndef VAPI_CORE_FUTURE_I_COMPRESSOR_HPP
#define VAPI_CORE_FUTURE_I_COMPRESSOR_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <span>
#include <vector>

namespace vapi::future {

class ICompressor {
public:
    virtual ~ICompressor() = default;
    [[nodiscard]] virtual Result<std::vector<u8>> compress(std::span<const u8> src) = 0;
    [[nodiscard]] virtual Result<std::vector<u8>> decompress(std::span<const u8> src) = 0;
    [[nodiscard]] virtual Result<void> decompressInto(std::span<const u8> src, std::span<u8> out) = 0;
};

} // namespace vapi::future

#endif // VAPI_CORE_FUTURE_I_COMPRESSOR_HPP

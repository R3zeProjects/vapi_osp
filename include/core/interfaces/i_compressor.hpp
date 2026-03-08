/** @file i_compressor.hpp
 *  @deprecated Moved to core/interfaces/future/i_compressor.hpp (namespace vapi::future).
 *  This header is kept for source compatibility but ICompressor has no shipping implementation.
 *  Include future/i_compressor.hpp directly if you plan to implement this interface. */

#ifndef VAPI_CORE_I_COMPRESSOR_HPP
#define VAPI_CORE_I_COMPRESSOR_HPP

#include "core/interfaces/future/i_compressor.hpp"

namespace vapi {
    using ICompressor = vapi::future::ICompressor;
} // namespace vapi

#endif // VAPI_CORE_I_COMPRESSOR_HPP

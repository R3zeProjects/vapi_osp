/** @file i_serializable.hpp
 *  @deprecated Moved to core/interfaces/future/i_serializable.hpp (namespace vapi::future).
 *  This header is kept for source compatibility but ISerializable/IClonable/INamed have no
 *  shipping implementations. Include future/i_serializable.hpp directly to implement these. */

#ifndef VAPI_CORE_I_SERIALIZABLE_HPP
#define VAPI_CORE_I_SERIALIZABLE_HPP

#include "core/interfaces/future/i_serializable.hpp"

namespace vapi {
    using ISerializable = vapi::future::ISerializable;
    using IClonable     = vapi::future::IClonable;
    using INamed        = vapi::future::INamed;
} // namespace vapi

#endif // VAPI_CORE_I_SERIALIZABLE_HPP

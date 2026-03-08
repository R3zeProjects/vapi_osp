/** @file future/i_serializable.hpp
 *  @brief Reserved interfaces: ISerializable, IClonable, INamed — no implementation yet.
 *  @note These interfaces are intentionally not part of the public vapi.hpp include.
 *        They are provided for future backend implementors. Signatures may change. */

#ifndef VAPI_CORE_FUTURE_I_SERIALIZABLE_HPP
#define VAPI_CORE_FUTURE_I_SERIALIZABLE_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <span>
#include <vector>
#include <memory>
#include <string_view>

namespace vapi::future {

class ISerializable {
public:
    virtual ~ISerializable() = default;
    [[nodiscard]] virtual Result<std::vector<u8>> serialize() const = 0;
    [[nodiscard]] virtual Result<void> deserialize(std::span<const u8> data) = 0;
};

class IClonable {
public:
    virtual ~IClonable() = default;
    [[nodiscard]] virtual std::unique_ptr<IClonable> clone() const = 0;
};

class INamed {
public:
    virtual ~INamed() = default;
    virtual void setName(std::string_view name) = 0;
    [[nodiscard]] virtual std::string_view name() const = 0;
};

} // namespace vapi::future

#endif // VAPI_CORE_FUTURE_I_SERIALIZABLE_HPP

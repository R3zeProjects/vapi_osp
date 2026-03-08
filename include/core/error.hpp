#ifndef VAPI_CORE_ERROR_HPP
#define VAPI_CORE_ERROR_HPP

/** @file error.hpp
 *  @brief Error type, error codes (errors::, file_errors), Result, errorMessage(), VAPI_TRY macros.
 *
 *  ## Error convention
 *
 *  - **Error codes**: vapi::Error holds a 32-bit code (0 = success). Codes from vapi::errors:: (0x1xxx),
 *    vapi::errors::file:: (0x2xxx), and vapi::gpu_errors (0x4xxx) define the category; for new scenarios
 *    add codes to the appropriate namespace and update errorMessage().
 *
 *  - **Result<T>**: alias for std::expected<T, Error>. Functions return Result<T> when they can fail;
 *    success is *res, error is res.error(). Check: if (!res) return std::unexpected(res.error());.
 *
 *  - **errorMessage(Error)**: returns a human-readable description (std::string_view) for logging
 *    and user messages. Does not throw.
 *  - **formatError(Error, string_view context)**: returns a "context: message" string for logs with call-site context.
 *
 *  - **VAPI_TRY(expr)** (GCC/Clang): unwraps Result; on error returns from function with return std::unexpected(...);
 *    on success yields the value. Use: auto x = VAPI_TRY(someResult());
 *
 *  - **VAPI_TRY(var, expr)**: same, result assigned to var. On MSVC both forms are supported:
 *    VAPI_TRY(expr) (in return) and VAPI_TRY(var, expr). If the expression contains a comma, use the two-argument form.
 *
 *  - **VAPI_TRY_ASSIGN(var, expr)**: explicit assign to var with error return on failure.
 *
 *  - **VAPI_TRY_VOID(expr)**: for Result<void>; on error returns from function, on success continues.
 */

#include "core/types.hpp"
#include <string>
#include <string_view>

namespace vapi {

namespace errors {
    inline constexpr Error Ok                 {0x0000};
    inline constexpr Error Init               {0x1001};
    inline constexpr Error NotInitialized     {0x1002};
    inline constexpr Error InvalidHandle     {0x1003};
    inline constexpr Error InvalidId         {0x1004};
    inline constexpr Error LimitReached      {0x1005};
    inline constexpr Error IdOverflow         {0x1006};
    inline constexpr Error Timeout            {0x1007};
    inline constexpr Error NotReady           {0x1008};
    inline constexpr Error OutOfMemory       {0x1009};
    inline constexpr Error InvalidArgument    {0x100A};
    inline constexpr Error NotSupported      {0x100B};
    inline constexpr Error AlreadyInitialized {0x100C};
    inline constexpr Error NotImplemented    {0x100D};
    inline constexpr Error CreateWindow      {0x100E};
    inline constexpr Error IOError           {0x100F};
    inline constexpr Error SerializationFail {0x1010};
    inline constexpr Error OutOfRange        {0x1011};

    namespace file {
        inline constexpr Error NotFound         {0x2001};
        inline constexpr Error OpenFailed       {0x2002};
        inline constexpr Error ReadFailed       {0x2003};
        inline constexpr Error WriteFailed      {0x2004};
        inline constexpr Error PermissionDenied {0x2005};
        inline constexpr Error NotDirectory     {0x2006};
    }
}

namespace file_errors = errors::file;

/** @brief Returns a single human-readable description for the given error code.
 *  @param e Error code (e.g. from Result::error()).
 *  @return Non-null string view; never throws. */
[[nodiscard]] inline std::string_view errorMessage(Error e) noexcept {
    using namespace errors;
    if (e.ok()) return "Success";
    switch (e.value()) {
        case 0x1001: return "Initialization failed";
        case 0x1002: return "Not initialized";
        case 0x1003: return "Invalid handle";
        case 0x1004: return "Invalid ID";
        case 0x1005: return "Limit reached";
        case 0x1006: return "ID overflow";
        case 0x1007: return "Timeout";
        case 0x1008: return "Not ready";
        case 0x1009: return "Out of memory";
        case 0x100A: return "Invalid argument";
        case 0x100B: return "Not supported";
        case 0x100C: return "Already initialized";
        case 0x100D: return "Not implemented";
        case 0x100E: return "Create window failed";
        case 0x100F: return "I/O error";
        case 0x1010: return "Serialization failed";
        case 0x2001: return "File not found";
        case 0x2002: return "File open failed";
        case 0x2003: return "File read failed";
        case 0x2004: return "File write failed";
        case 0x2005: return "Permission denied";
        case 0x2006: return "Not a directory";
        case 0x4001: return "Vulkan operation failed";
        case 0x4002: return "Device not found";
        case 0x4003: return "No suitable queue";
        case 0x4004: return "Surface creation failed";
        case 0x4005: return "Swapchain creation failed";
        case 0x4006: return "Swapchain out of date";
        case 0x4007: return "Image acquire failed";
        case 0x4008: return "Present failed";
        case 0x4009: return "Fence timeout";
        case 0x400A: return "Command pool creation failed";
        case 0x400B: return "Command buffer allocation failed";
        case 0x400C: return "Allocator creation failed";
        case 0x400D: return "Allocation failed";
        case 0x400E: return "Map failed";
        case 0x400F: return "Validation layer not available";
        case 0x4010: return "Extension not available";
        case 0x4011: return "Shader module creation failed";
        case 0x4012: return "Pipeline creation failed";
        case 0x4013: return "Render pass creation failed";
        case 0x4014: return "Framebuffer creation failed";
        case 0x4015: return "Descriptor set layout failed";
        case 0x4016: return "Descriptor pool failed";
        case 0x4017: return "Buffer creation failed";
        case 0x4018: return "Image creation failed";
        case 0x4019: return "Sampler creation failed";
        case 0x401A: return "Semaphore creation failed";
        case 0x401B: return "Fence creation failed";
        case 0x401C: return "Query pool creation failed";
        default:     return "Unknown error";
    }
}

/** @brief Format error with context for logging (e.g. "FontRenderApp::init: Initialization failed").
 *  @param e Error code.
 *  @param context Call site or component name.
 *  @return Formatted string; never throws. */
[[nodiscard]] inline std::string formatError(Error e, std::string_view context) noexcept {
    if (context.empty()) return std::string(errorMessage(e));
    return std::string(context) + ": " + std::string(errorMessage(e));
}

} // namespace vapi

#if defined(__GNUC__) || defined(__clang__)
/** Unwrap Result; on error returns from the current function. Yields the value. */
#define VAPI_TRY(expr) \
    ({ auto _res = (expr); \
       if (!_res) return std::unexpected(_res.error()); \
       std::move(*_res); })
#else
/** On MSVC: VAPI_TRY(expr) for return; VAPI_TRY(var, expr) for assignment. (Expression with comma: use two-arg form.) */
#define VAPI_TRY_NARGS_(_1, _2, N, ...) N
#define VAPI_TRY_NARGS(...) VAPI_TRY_NARGS_(__VA_ARGS__, 2, 1)
#define VAPI_TRY_1(expr) (([&]() -> decltype(expr) { auto _r = (expr); if (!_r) return std::unexpected(_r.error()); return std::move(*_r); })())
#define VAPI_TRY_2(var, expr) VAPI_TRY_ASSIGN(var, expr)
#define VAPI_TRY_SELECT(N) VAPI_TRY_SELECT_(N)
#define VAPI_TRY_SELECT_(N) VAPI_TRY_##N
#define VAPI_TRY(...) VAPI_TRY_SELECT(VAPI_TRY_NARGS(__VA_ARGS__))(__VA_ARGS__)
#endif

/** Assign result to var; on error returns from the current function. */
#define VAPI_TRY_ASSIGN(var, expr) \
    auto var = (expr); \
    if (!(var)) return std::unexpected((var).error())

/** Evaluate expression returning Result<void>; on error returns from the current function. */
#define VAPI_TRY_VOID(expr) \
    do { auto _res = (expr); \
         if (!_res) return std::unexpected(_res.error()); \
    } while(0)

#endif // VAPI_CORE_ERROR_HPP

/** @file i_gpu_abi.hpp
 *  @brief Cross-API GPU abstraction: format, shader stage, vertex layout.
 *  Backend-agnostic descriptors; backends (Vulkan, D3D, Metal) map these to native types. */

#ifndef VAPI_CORE_I_GPU_ABI_HPP
#define VAPI_CORE_I_GPU_ABI_HPP

#include "core/types.hpp"
#include <array>
#include <vector>

namespace vapi {

// ─── Pixel format (cross-API) ─────────────────────────────────────────────

enum class PixelFormat : u16 {
    Undefined = 0,
    R8G8B8A8_UNORM,
    R8G8B8A8_SRGB,
    B8G8R8A8_UNORM,
    B8G8R8A8_SRGB,
    R32G32B32A32_SFLOAT,
    R32G32B32_SFLOAT,
    R32G32_SFLOAT,
    R32_SFLOAT,
    D32_SFLOAT,
    D24_UNORM_S8_UINT,
};

// ─── Shader stage (cross-API) ───────────────────────────────────────────────

enum class ShaderStage : u8 {
    Vertex = 0,
    Fragment,
    Compute,
    Geometry,
    TessControl,
    TessEval,
};

/** Combined stage flags (bitmask). */
enum class ShaderStageFlags : u16 {
    None    = 0,
    Vertex  = 1 << 0,
    Fragment = 1 << 1,
    Compute = 1 << 2,
    AllGraphics = Vertex | Fragment,
    All = Vertex | Fragment | Compute,
};
inline ShaderStageFlags operator|(ShaderStageFlags a, ShaderStageFlags b) {
    return static_cast<ShaderStageFlags>(static_cast<u16>(a) | static_cast<u16>(b));
}
inline ShaderStageFlags operator&(ShaderStageFlags a, ShaderStageFlags b) {
    return static_cast<ShaderStageFlags>(static_cast<u16>(a) & static_cast<u16>(b));
}

// ─── Vertex input (cross-API) ──────────────────────────────────────────────

enum class VertexInputRate : u8 {
    Vertex   = 0,
    Instance = 1,
};

struct VertexAttributeDesc {
    u32          location{0};
    u32          binding{0};
    PixelFormat  format{PixelFormat::R32G32B32A32_SFLOAT};
    u32          offset{0};
};

struct VertexBindingDesc {
    u32             binding{0};
    u32             stride{0};
    VertexInputRate inputRate{VertexInputRate::Vertex};
};

// ─── Primitive topology (cross-API) ────────────────────────────────────────

enum class PrimitiveTopology : u8 {
    TriangleList  = 0,
    TriangleStrip,
    LineList,
    LineStrip,
    PointList,
};

// ─── Compare op (depth/stencil) ─────────────────────────────────────────────

enum class CompareOp : u8 {
    Never = 0, Less, Equal, LessOrEqual, Greater, NotEqual, GreaterOrEqual, Always
};

} // namespace vapi

#endif // VAPI_CORE_I_GPU_ABI_HPP

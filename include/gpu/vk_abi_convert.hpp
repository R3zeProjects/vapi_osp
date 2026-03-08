/** @file vk_abi_convert.hpp
 *  @brief Convert cross-API ABI types (i_gpu_abi.hpp) to Vulkan types. */

#ifndef VAPI_GPU_VK_ABI_CONVERT_HPP
#define VAPI_GPU_VK_ABI_CONVERT_HPP

#include "core/interfaces/i_gpu_abi.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace vapi {

[[nodiscard]] inline VkFormat toVkFormat(PixelFormat f) {
    switch (f) {
        case PixelFormat::Undefined:           return VK_FORMAT_UNDEFINED;
        case PixelFormat::R8G8B8A8_UNORM:      return VK_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::R8G8B8A8_SRGB:      return VK_FORMAT_R8G8B8A8_SRGB;
        case PixelFormat::B8G8R8A8_UNORM:     return VK_FORMAT_B8G8R8A8_UNORM;
        case PixelFormat::B8G8R8A8_SRGB:      return VK_FORMAT_B8G8R8A8_SRGB;
        case PixelFormat::R32G32B32A32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
        case PixelFormat::R32G32B32_SFLOAT:   return VK_FORMAT_R32G32B32_SFLOAT;
        case PixelFormat::R32G32_SFLOAT:      return VK_FORMAT_R32G32_SFLOAT;
        case PixelFormat::R32_SFLOAT:         return VK_FORMAT_R32_SFLOAT;
        case PixelFormat::D32_SFLOAT:         return VK_FORMAT_D32_SFLOAT;
        case PixelFormat::D24_UNORM_S8_UINT:   return VK_FORMAT_D24_UNORM_S8_UINT;
        default:                               return VK_FORMAT_UNDEFINED;
    }
}

[[nodiscard]] inline VkShaderStageFlagBits toVkShaderStage(ShaderStage s) {
    switch (s) {
        case ShaderStage::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
        default:                    return VK_SHADER_STAGE_VERTEX_BIT;
    }
}

[[nodiscard]] inline VkShaderStageFlags toVkShaderStageFlags(ShaderStageFlags f) {
    VkShaderStageFlags vk = 0;
    if ((f & ShaderStageFlags::Vertex) != ShaderStageFlags::None)   vk |= VK_SHADER_STAGE_VERTEX_BIT;
    if ((f & ShaderStageFlags::Fragment) != ShaderStageFlags::None)   vk |= VK_SHADER_STAGE_FRAGMENT_BIT;
    if ((f & ShaderStageFlags::Compute) != ShaderStageFlags::None)  vk |= VK_SHADER_STAGE_COMPUTE_BIT;
    return vk;
}

[[nodiscard]] inline VkVertexInputRate toVkVertexInputRate(VertexInputRate r) {
    return r == VertexInputRate::Instance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
}

[[nodiscard]] inline VkVertexInputAttributeDescription toVkVertexInputAttributeDescription(const VertexAttributeDesc& a) {
    return {
        a.location,
        a.binding,
        toVkFormat(a.format),
        a.offset
    };
}

[[nodiscard]] inline VkVertexInputBindingDescription toVkVertexInputBindingDescription(const VertexBindingDesc& b) {
    return {
        b.binding,
        b.stride,
        toVkVertexInputRate(b.inputRate)
    };
}

[[nodiscard]] inline VkPrimitiveTopology toVkPrimitiveTopology(PrimitiveTopology t) {
    switch (t) {
        case PrimitiveTopology::TriangleList:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case PrimitiveTopology::LineList:     return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case PrimitiveTopology::LineStrip:    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case PrimitiveTopology::PointList:    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        default:                              return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

[[nodiscard]] inline VkCompareOp toVkCompareOp(CompareOp op) {
    switch (op) {
        case CompareOp::Never:         return VK_COMPARE_OP_NEVER;
        case CompareOp::Less:          return VK_COMPARE_OP_LESS;
        case CompareOp::Equal:         return VK_COMPARE_OP_EQUAL;
        case CompareOp::LessOrEqual:   return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::Greater:      return VK_COMPARE_OP_GREATER;
        case CompareOp::NotEqual:      return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOp::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOp::Always:        return VK_COMPARE_OP_ALWAYS;
        default:                       return VK_COMPARE_OP_LESS;
    }
}

/** Build Vulkan vertex attribute list from cross-API descriptors. */
[[nodiscard]] inline std::vector<VkVertexInputAttributeDescription> toVkVertexInputAttributes(
    const VertexAttributeDesc* attributes, u32 count) {
    std::vector<VkVertexInputAttributeDescription> out;
    out.reserve(count);
    for (u32 i = 0; i < count; ++i)
        out.push_back(toVkVertexInputAttributeDescription(attributes[i]));
    return out;
}

/** Build Vulkan vertex binding list from cross-API descriptors. */
[[nodiscard]] inline std::vector<VkVertexInputBindingDescription> toVkVertexInputBindings(
    const VertexBindingDesc* bindings, u32 count) {
    std::vector<VkVertexInputBindingDescription> out;
    out.reserve(count);
    for (u32 i = 0; i < count; ++i)
        out.push_back(toVkVertexInputBindingDescription(bindings[i]));
    return out;
}

} // namespace vapi

#endif // VAPI_GPU_VK_ABI_CONVERT_HPP

/** @file vertex.hpp @brief Vertex layouts and vertex input state for graphics pipelines. */
#ifndef VAPI_RENDER_VERTEX_HPP
#define VAPI_RENDER_VERTEX_HPP

#include "core/types.hpp"
#include <vulkan/vulkan.h>
#include <array>
#include <vector>

namespace vapi {

/** Position-only vertex. */
struct VertexP {
    vec3 position;
    static constexpr u32 offset_position = 0;
    static VkVertexInputBindingDescription binding(u32 bind = 0) {
        return {bind, sizeof(VertexP), VK_VERTEX_INPUT_RATE_VERTEX};
    }
    static std::array<VkVertexInputAttributeDescription, 1> attributes(u32 bind = 0) {
        return {{{0, bind, VK_FORMAT_R32G32B32_SFLOAT, offset_position}}};
    }
};

/** 2D position, UV, color — for UI quads (e.g. text glyphs, solid rects). */
struct VertexP2UC {
    vec2   position;
    vec2   uv;
    color4 color;
    static constexpr u32 offset_position = 0;
    static constexpr u32 offset_uv       = sizeof(vec2);
    static constexpr u32 offset_color    = sizeof(vec2) + sizeof(vec2);
    static VkVertexInputBindingDescription binding(u32 bind = 0) {
        return {bind, sizeof(VertexP2UC), VK_VERTEX_INPUT_RATE_VERTEX};
    }
    static std::array<VkVertexInputAttributeDescription, 3> attributes(u32 bind = 0) {
        return {{
            {0, bind, VK_FORMAT_R32G32_SFLOAT,     offset_position},
            {1, bind, VK_FORMAT_R32G32_SFLOAT,     offset_uv},
            {2, bind, VK_FORMAT_R32G32B32A32_SFLOAT, offset_color},
        }};
    }
};

/** Position and color. */
struct VertexPC {
    vec3   position;
    color4 color;
    static constexpr u32 offset_position = 0;
    static constexpr u32 offset_color    = sizeof(vec3);
    static VkVertexInputBindingDescription binding(u32 bind = 0) {
        return {bind, sizeof(VertexPC), VK_VERTEX_INPUT_RATE_VERTEX};
    }
    static std::array<VkVertexInputAttributeDescription, 2> attributes(u32 bind = 0) {
        return {{
            {0, bind, VK_FORMAT_R32G32B32_SFLOAT,    offset_position},
            {1, bind, VK_FORMAT_R32G32B32A32_SFLOAT, offset_color},
        }};
    }
};

/** Position, normal, UV. */
struct VertexPNU {
    vec3 position;
    vec3 normal;
    vec2 uv;
    static constexpr u32 offset_position = 0;
    static constexpr u32 offset_normal   = sizeof(vec3);
    static constexpr u32 offset_uv       = 2u * sizeof(vec3);
    static VkVertexInputBindingDescription binding(u32 bind = 0) {
        return {bind, sizeof(VertexPNU), VK_VERTEX_INPUT_RATE_VERTEX};
    }
    static std::array<VkVertexInputAttributeDescription, 3> attributes(u32 bind = 0) {
        return {{
            {0, bind, VK_FORMAT_R32G32B32_SFLOAT, offset_position},
            {1, bind, VK_FORMAT_R32G32B32_SFLOAT, offset_normal},
            {2, bind, VK_FORMAT_R32G32_SFLOAT,    offset_uv},
        }};
    }
};

/** Full vertex: position, normal, UV, tangent, color. */
struct VertexFull {
    vec3   position;
    vec3   normal;
    vec2   uv;
    vec4   tangent;
    color4 color;
    static constexpr u32 offset_position = 0;
    static constexpr u32 offset_normal   = sizeof(vec3);
    static constexpr u32 offset_uv       = 2u * sizeof(vec3);
    static constexpr u32 offset_tangent  = 2u * sizeof(vec3) + sizeof(vec2);
    static constexpr u32 offset_color    = 2u * sizeof(vec3) + sizeof(vec2) + sizeof(vec4);
    static VkVertexInputBindingDescription binding(u32 bind = 0) {
        return {bind, sizeof(VertexFull), VK_VERTEX_INPUT_RATE_VERTEX};
    }
    static std::array<VkVertexInputAttributeDescription, 5> attributes(u32 bind = 0) {
        return {{
            {0, bind, VK_FORMAT_R32G32B32_SFLOAT,    offset_position},
            {1, bind, VK_FORMAT_R32G32B32_SFLOAT,    offset_normal},
            {2, bind, VK_FORMAT_R32G32_SFLOAT,       offset_uv},
            {3, bind, VK_FORMAT_R32G32B32A32_SFLOAT, offset_tangent},
            {4, bind, VK_FORMAT_R32G32B32A32_SFLOAT, offset_color},
        }};
    }
};

/** Build vertex input state from a vertex type V (e.g. VertexPC, VertexP2UC).
 *  @tparam V Vertex struct with static binding() and attributes(). */
template<typename V>
struct VertexInputState {
    VkVertexInputBindingDescription binding;
    std::vector<VkVertexInputAttributeDescription> attributes;

    /** @param bind Binding index (default 0). */
    explicit VertexInputState(u32 bind = 0) : binding(V::binding(bind)) {
        auto attrs = V::attributes(bind);
        attributes.assign(attrs.begin(), attrs.end());
    }

    /** @return VkPipelineVertexInputStateCreateInfo for pipeline vertex input state. */
    [[nodiscard]] VkPipelineVertexInputStateCreateInfo createInfo() const {
        VkPipelineVertexInputStateCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        ci.vertexBindingDescriptionCount = 1;
        ci.pVertexBindingDescriptions = &binding;
        ci.vertexAttributeDescriptionCount = static_cast<u32>(attributes.size());
        ci.pVertexAttributeDescriptions = attributes.data();
        return ci;
    }
};

} // namespace vapi

#endif

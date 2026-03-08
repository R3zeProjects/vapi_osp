/** @file pipeline_builder.hpp
 *  @brief Fluent builders for Vulkan graphics and compute pipelines (dynamic rendering). */

#ifndef VAPI_RENDER_PIPELINE_BUILDER_HPP
#define VAPI_RENDER_PIPELINE_BUILDER_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "gpu/vk_errors.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace vapi {

// ── Graphics Pipeline Builder ─────────────────────────────────

class GraphicsPipelineBuilder {
public:
    GraphicsPipelineBuilder();

    // ── Shaders ───────────────────────────────────────────────
    /** @param stage Valid VkPipelineShaderStageCreateInfo (module must outlive build). */
    GraphicsPipelineBuilder& addShaderStage(VkPipelineShaderStageCreateInfo stage);
    /** @param stages Vector of stages; modules must outlive build(). */
    GraphicsPipelineBuilder& setShaderStages(std::vector<VkPipelineShaderStageCreateInfo> stages);

    // ── Vertex Input ──────────────────────────────────────────
    /** @param bindings Pointer to at least bindingCount bindings; must be valid until build().
     *  @param attributes Pointer to at least attributeCount attributes; must be valid until build(). */
    GraphicsPipelineBuilder& setVertexInput(
        const VkVertexInputBindingDescription* bindings, u32 bindingCount,
        const VkVertexInputAttributeDescription* attributes, u32 attributeCount);

    /** Set vertex input from vertex type V (binding + attributes). */
    template<typename V>
    GraphicsPipelineBuilder& setVertexInput() {
        m_vertexBinding = V::binding();
        auto attrs = V::attributes();
        m_vertexAttributes.assign(attrs.begin(), attrs.end());
        return *this;
    }

    // ── Topology ──────────────────────────────────────────────
    GraphicsPipelineBuilder& setTopology(VkPrimitiveTopology topo);

    // ── Viewport / Scissor (dynamic by default) ───────────────
    GraphicsPipelineBuilder& setViewportCount(u32 count);

    // ── Rasterization ─────────────────────────────────────────
    GraphicsPipelineBuilder& setPolygonMode(VkPolygonMode mode);
    GraphicsPipelineBuilder& setCullMode(VkCullModeFlags cull, VkFrontFace front = VK_FRONT_FACE_COUNTER_CLOCKWISE);
    GraphicsPipelineBuilder& setLineWidth(f32 width);
    GraphicsPipelineBuilder& setDepthClamp(bool enable);

    // ── Depth / Stencil ───────────────────────────────────────
    GraphicsPipelineBuilder& setDepthTest(bool enable, bool write = true, VkCompareOp op = VK_COMPARE_OP_LESS);
    GraphicsPipelineBuilder& setDepthBounds(bool enable, f32 min = 0.f, f32 max = 1.f);
    GraphicsPipelineBuilder& setStencilTest(bool enable);

    // ── Blending ──────────────────────────────────────────────
    GraphicsPipelineBuilder& setBlendAttachment(VkPipelineColorBlendAttachmentState att);
    GraphicsPipelineBuilder& setAlphaBlending();  // preset: standard alpha blend
    GraphicsPipelineBuilder& setNoBlending();      // preset: no blend (opaque)

    // ── Multisampling ─────────────────────────────────────────
    GraphicsPipelineBuilder& setMultisample(VkSampleCountFlagBits samples);

    // ── Dynamic State ─────────────────────────────────────────
    GraphicsPipelineBuilder& addDynamicState(VkDynamicState state);

    // ── Layout ────────────────────────────────────────────────
    GraphicsPipelineBuilder& setPipelineLayout(VkPipelineLayout layout);
    /** @param layouts Pointer to at least count layouts; valid until build(). */
    GraphicsPipelineBuilder& setDescriptorSetLayouts(const VkDescriptorSetLayout* layouts, u32 count);
    GraphicsPipelineBuilder& setPushConstantRange(VkShaderStageFlags stages, u32 offset, u32 size);

    // ── Render targets (dynamic rendering — Vulkan 1.3) ──────
    /** @param formats Pointer to at least count formats; valid until build(). */
    GraphicsPipelineBuilder& setColorFormats(const VkFormat* formats, u32 count);
    GraphicsPipelineBuilder& setColorFormat(VkFormat format);
    GraphicsPipelineBuilder& setDepthFormat(VkFormat format);
    GraphicsPipelineBuilder& setStencilFormat(VkFormat format);

    // ── Build ─────────────────────────────────────────────────
    /** Build pipeline. device must be valid; pipelineCache optional (e.g. PipelineCache::handle()) to reuse compiled state.
     *  @param device Valid VkDevice; must remain valid for the returned pipeline lifetime (caller owns pipeline).
     *  @param pipelineCache Optional; if valid, may be used to speed up creation.
     *  @return Created VkPipeline, or error. Caller must destroy pipeline with vkDestroyPipeline. */
    [[nodiscard]] Result<VkPipeline> build(VkDevice device, VkPipelineCache pipelineCache = VK_NULL_HANDLE);

    /** Build pipeline and optionally create pipeline layout. outLayout receives layout (caller must destroy if builder created it).
     *  @param outLayout On success, set to the layout used (either previously set or newly created).
     *  @return Created VkPipeline, or error. */
    [[nodiscard]] Result<VkPipeline> build(VkDevice device, VkPipelineLayout& outLayout, VkPipelineCache pipelineCache = VK_NULL_HANDLE);

private:
    // Shader stages
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

    // Vertex input
    VkVertexInputBindingDescription m_vertexBinding{};
    std::vector<VkVertexInputAttributeDescription> m_vertexAttributes;
    bool m_hasVertexInput{false};

    // Fixed states
    VkPrimitiveTopology                    m_topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    VkPolygonMode                          m_polygonMode{VK_POLYGON_MODE_FILL};
    VkCullModeFlags                        m_cullMode{VK_CULL_MODE_BACK_BIT};
    VkFrontFace                            m_frontFace{VK_FRONT_FACE_COUNTER_CLOCKWISE};
    f32                                    m_lineWidth{1.f};
    bool                                   m_depthClamp{false};
    VkSampleCountFlagBits                  m_samples{VK_SAMPLE_COUNT_1_BIT};

    // Depth / stencil
    bool        m_depthTestEnable{true};
    bool        m_depthWriteEnable{true};
    VkCompareOp m_depthCompareOp{VK_COMPARE_OP_LESS};
    bool        m_depthBoundsEnable{false};
    f32         m_depthBoundsMin{0.f};
    f32         m_depthBoundsMax{1.f};
    bool        m_stencilEnable{false};

    // Blend
    VkPipelineColorBlendAttachmentState m_blendAttachment{};
    bool m_blendSet{false};

    // Dynamic state
    std::vector<VkDynamicState> m_dynamicStates;

    // Layout
    VkPipelineLayout m_layout{VK_NULL_HANDLE};
    std::vector<VkDescriptorSetLayout> m_descLayouts;
    std::vector<VkPushConstantRange> m_pushConstants;

    // Dynamic rendering (Vulkan 1.3)
    std::vector<VkFormat> m_colorFormats;
    VkFormat              m_depthFormat{VK_FORMAT_UNDEFINED};
    VkFormat              m_stencilFormat{VK_FORMAT_UNDEFINED};

    u32 m_viewportCount{1};
};

// ── Compute Pipeline Builder ──────────────────────────────────

class ComputePipelineBuilder {
public:
    /** @param stage Valid stage (module must outlive build()). */
    ComputePipelineBuilder& setShader(VkPipelineShaderStageCreateInfo stage);
    ComputePipelineBuilder& setPipelineLayout(VkPipelineLayout layout);
    /** @param layouts Pointer to at least count layouts; valid until build(). */
    ComputePipelineBuilder& setDescriptorSetLayouts(const VkDescriptorSetLayout* layouts, u32 count);
    ComputePipelineBuilder& setPushConstantRange(VkShaderStageFlags stages, u32 offset, u32 size);

    /** @param device Valid VkDevice; caller owns returned pipeline.
     *  @return Created VkPipeline or error. */
    [[nodiscard]] Result<VkPipeline> build(VkDevice device, VkPipelineCache pipelineCache = VK_NULL_HANDLE);
    /** Same as build(device, pipelineCache) but outLayout receives the layout used. */
    [[nodiscard]] Result<VkPipeline> build(VkDevice device, VkPipelineLayout& outLayout, VkPipelineCache pipelineCache = VK_NULL_HANDLE);

private:
    VkPipelineShaderStageCreateInfo m_shaderStage{};
    VkPipelineLayout m_layout{VK_NULL_HANDLE};
    std::vector<VkDescriptorSetLayout> m_descLayouts;
    std::vector<VkPushConstantRange> m_pushConstants;
};

} // namespace vapi

#endif

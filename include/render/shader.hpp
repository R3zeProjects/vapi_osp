/** @file shader.hpp @brief Shader module and program types for Vulkan. */
#ifndef VAPI_RENDER_SHADER_HPP
#define VAPI_RENDER_SHADER_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "core/interfaces/i_gpu_abi.hpp"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <filesystem>
#include <optional>

namespace vapi {

/** Default max SPIR-V size (4 MiB). Used when maxSpirvBytes is not specified. */
constexpr usize kDefaultMaxSpirvBytes = 4ull * 1024 * 1024;

/** Options for loading shaders (file or memory). Use to avoid long parameter lists. */
struct LoadShaderOptions {
    std::optional<std::filesystem::path> allowedRoot;
    usize maxSpirvBytes{kDefaultMaxSpirvBytes};
};

/** ShaderStage is defined in core/interfaces/i_gpu_abi.hpp (Vertex, Fragment, Compute, Geometry, TessControl, TessEval). */

/** RAII wrapper for VkShaderModule. */
class ShaderModule {
public:
    ShaderModule() = default;
    ~ShaderModule();

    ShaderModule(const ShaderModule&) = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;
    ShaderModule(ShaderModule&&) noexcept;
    ShaderModule& operator=(ShaderModule&&) noexcept;

    /** Load SPIR-V from file. If allowedRoot is set, path must resolve under it (safe path).
     *  @param device Valid VkDevice; must remain valid for the lifetime of this module (until destroy() or destruction).
     *  @param path Path to .spv file.
     *  @param stage Shader stage.
     *  @param allowedRoot If set, path must be under this root.
     *  @param maxSpirvBytes Max file size (default 4 MiB).
     *  @return Ok on success.
     *  @pre device != VK_NULL_HANDLE. */
    [[nodiscard]] Result<void> loadFromFile(VkDevice device, const std::filesystem::path& path, ShaderStage stage,
                                            std::optional<std::filesystem::path> allowedRoot = std::nullopt,
                                            usize maxSpirvBytes = kDefaultMaxSpirvBytes);
    /** Overload: options in struct (e.g. opts.allowedRoot, opts.maxSpirvBytes). Same device lifetime requirement. */
    [[nodiscard]] Result<void> loadFromFile(VkDevice device, const std::filesystem::path& path, ShaderStage stage,
                                            const LoadShaderOptions& opts);
    /** Load SPIR-V from memory. Replaces any existing module.
     *  @param device Valid VkDevice; must remain valid for the lifetime of this module.
     *  @param spirv SPIR-V code (4-byte aligned).
     *  @param maxSpirvBytes Max size (default 4 MiB).
     *  @return Ok on success.
     *  @pre device != VK_NULL_HANDLE; spirv non-empty and within size limit. */
    [[nodiscard]] Result<void> loadFromMemory(VkDevice device, std::span<const u32> spirv, ShaderStage stage,
                                              usize maxSpirvBytes = kDefaultMaxSpirvBytes);
    /** Overload: options in struct. Same device lifetime requirement. */
    [[nodiscard]] Result<void> loadFromMemory(VkDevice device, std::span<const u32> spirv, ShaderStage stage,
                                              const LoadShaderOptions& opts);

    /** Destroy shader module. Safe when not loaded. Use same device as for load. */
    void destroy();

    [[nodiscard]] VkShaderModule handle() const { return m_module; }
    [[nodiscard]] ShaderStage stage() const { return m_stage; }
    [[nodiscard]] VkShaderStageFlagBits vkStage() const;
    [[nodiscard]] const std::string& name() const { return m_name; }
    void setName(std::string n) { m_name = std::move(n); }

    /** Build VkPipelineShaderStageCreateInfo for pipeline creation. Module must outlive pipeline build. */
    [[nodiscard]] VkPipelineShaderStageCreateInfo stageInfo(const char* entryPoint = "main") const;

    explicit operator bool() const { return m_module != VK_NULL_HANDLE; }

private:
    VkShaderModule m_module{VK_NULL_HANDLE};
    VkDevice       m_device{VK_NULL_HANDLE};
    ShaderStage    m_stage{ShaderStage::Vertex};
    std::string    m_name;
};

/** Set of shader modules for one pipeline. */
struct ShaderProgram {
    ShaderModule vertex;
    ShaderModule fragment;
    ShaderModule compute;
    ShaderModule geometry;
    ShaderModule tessCtrl;
    ShaderModule tessEval;

    [[nodiscard]] std::vector<VkPipelineShaderStageCreateInfo> stages(const char* entry = "main") const;
};

} // namespace vapi

#endif

/** @file glslang_validator_compiler.hpp
 *  @brief IShaderSourceCompiler via glslangValidator (Vulkan SDK). Requires glslangValidator in PATH. */

#ifndef VAPI_CORE_TOOLS_GLSLANG_VALIDATOR_COMPILER_HPP
#define VAPI_CORE_TOOLS_GLSLANG_VALIDATOR_COMPILER_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "core/interfaces/i_gpu_tools.hpp"
#include <string>
#include <vector>

namespace vapi::tools {

/** Compiles GLSL to SPIR-V by invoking glslangValidator (must be in PATH, e.g. from Vulkan SDK). */
class GlslangValidatorCompiler : public IShaderSourceCompiler {
public:
    /** Optional path to glslangValidator executable. Empty = use "glslangValidator" from PATH. */
    void setExecutablePath(std::string path) { m_executablePath = std::move(path); }
    [[nodiscard]] const std::string& executablePath() const { return m_executablePath; }

    [[nodiscard]] Result<std::vector<u32>> compileFromSource(
        std::string_view source,
        std::string_view stageHint,
        ShaderSourceCompileOptions options = {}) override;
private:
    std::string m_executablePath;
};

} // namespace vapi::tools

#endif

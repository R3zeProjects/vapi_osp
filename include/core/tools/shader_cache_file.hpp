/** @file shader_cache_file.hpp
 *  @brief File-based shader cache (IShaderCache): loads SPIR-V from disk, caches by key (path). No compile step. */

#ifndef VAPI_CORE_TOOLS_SHADER_CACHE_FILE_HPP
#define VAPI_CORE_TOOLS_SHADER_CACHE_FILE_HPP

#include "core/interfaces/i_gpu_tools.hpp"
#include "core/file_utils.hpp"
#include "core/types.hpp"
#include <unordered_map>
#include <mutex>
#include <string>

namespace vapi::tools {

/** Loads SPIR-V from file; key is used as path (or basePath + key). Caches loaded bytecode in memory. */
class FileShaderCache : public IShaderCache {
public:
    /** Default max SPIR-V size per shader (4 MiB). */
    static constexpr usize kDefaultMaxSpirvBytes = 4ull * 1024 * 1024;

    /** If basePath is non-empty, getOrCompile(key, ...) loads file at basePath + key (or key as full path if basePath empty). */
    explicit FileShaderCache(std::string basePath = "");

    [[nodiscard]] Result<std::vector<u32>> getOrCompile(
        std::string_view key,
        std::string_view /*source*/,
        ShaderCompileOptions /*options*/) override;
    void clear() override;

    void setBasePath(std::string path) { m_basePath = std::move(path); }
    void setMaxSpirvBytes(usize max) { m_maxSpirvBytes = max; }
    [[nodiscard]] usize maxSpirvBytes() const { return m_maxSpirvBytes; }

private:
    std::string m_basePath;
    usize m_maxSpirvBytes{kDefaultMaxSpirvBytes};
    std::unordered_map<std::string, std::vector<u32>> m_cache;
    mutable std::mutex m_mutex;
};

} // namespace vapi::tools

#endif // VAPI_CORE_TOOLS_SHADER_CACHE_FILE_HPP

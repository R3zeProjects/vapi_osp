/** @file run_context.hpp
 *  @brief Shared builder for GPU backend + render context setup. Used by runUILoop and FontRenderApp. */

#ifndef VAPI_RENDER_RUN_CONTEXT_HPP
#define VAPI_RENDER_RUN_CONTEXT_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "gpu/i_gpu_backend.hpp"
#include <memory>
#include <vector>

namespace vapi {

/** Creates and initializes a Vulkan GPU backend with the given config. On success, context() and device/swapchain are valid. On failure, returns error (backend is not returned). */
[[nodiscard]] Result<std::unique_ptr<IGpuBackend>> createAndInitVulkanBackend(GpuBackendConfig config);

} // namespace vapi

#endif

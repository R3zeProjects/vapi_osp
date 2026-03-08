#ifndef VAPI_RESOURCE_HPP
#define VAPI_RESOURCE_HPP

/**
 * @file resource.hpp
 * @brief Master include for the resource module.
 *
 * Structure:
 *   resource/
 *   ├── resource.hpp              (this file)
 *   ├── resource_types.hpp        (GpuBuffer, GpuImage, GpuSampler, descriptors)
 *   ├── buffer_manager.hpp/cpp    (buffer creation, mapping, write)
 *   ├── image_manager.hpp/cpp     (image + view creation, depth image)
 *   ├── sampler_cache.hpp/cpp     (VkSampler cache by SamplerDesc)
 *   ├── descriptor_manager.hpp/cpp (layouts, pool, sets, write)
 *   └── staging.hpp/cpp            (CPU to GPU upload: buffer + image)
 */

#include "resource/resource_types.hpp"
#include "resource/buffer_manager.hpp"
#include "resource/image_manager.hpp"
#include "resource/sampler_cache.hpp"
#include "resource/descriptor_manager.hpp"
#include "resource/staging.hpp"

#endif // VAPI_RESOURCE_HPP

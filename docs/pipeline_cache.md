# What is pipeline cache?

**Pipeline cache** (in Vulkan: `VkPipelineCache`) is a cache of **compiled pipeline state**. When you create a `VkPipeline`, the driver compiles shaders and builds internal state. That work is expensive. The pipeline cache lets the driver store the result and reuse it later.

- **Save**: After creating pipelines (or at exit), you get the cache data with `vkGetPipelineCacheData()` and write it to a file.
- **Load**: On the next run, you create a `VkPipelineCache` from that file with `vkCreatePipelineCache()`. When you create pipelines using this cache, the driver can skip recompilation when the same pipeline was built before.

So **pipeline cache** = “save/load compiled pipeline state to avoid recompiling the same pipelines on every startup.” It shortens load times and reduces CPU/GPU work when the set of pipelines doesn’t change.

VAPI provides **PipelineCache** in `render/pipeline_cache.hpp`: create empty with `init(device)`, or load from file with `initFromFile(device, path)`; pass `cache.handle()` to `GraphicsPipelineBuilder::build(device, layout, cache.handle())` and `ComputePipelineBuilder::build(device, layout, cache.handle())`; at shutdown call `cache.saveToFile(path)` then `cache.shutdown(device)`.

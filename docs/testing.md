# Running tests (Testing)

Tests are built only when `VAPI_BUILD_TESTS=ON` (default). See [integration.md](integration.md).

## Test levels and CI behavior

| Level | Description | Requires display | In CI (headless) |
|-------|-------------|------------------|------------------|
| **Unit** | Logic without GPU/window (vec2, error, file_utils, emitter, layout, font metrics, …) | No | Run |
| **GPU unit** | Vulkan/wrappers without window (vk_gpu_profiler, vk_gpu_fence, gpu_abi, …) | No | Run |
| **Integration (display)** | Window/platform/render (platform_integration, render_correctness, buffer_manager_integration, …) | Yes | Skipped (exit 0 and stderr message) |
| **Headless** | `main_headless` (vapi_main --headless) | No | Run |

In a display-less environment, integration tests that need a window exit with code 0 and message “skip (no display)”. For a full run use an environment with a display or exclude display tests via `ctest -E "test_render_correctness|test_platform_integration|..."`.

## Full check (all unit and integration tests)

From the project root:

```
cmake -B build -S .
cmake --build build
ctest --test-dir build --output-on-failure
```

- **`ctest`** runs all targets registered with `add_test(NAME ... COMMAND ...)` in CMakeLists.txt.
- **`--output-on-failure`** prints test output when a test fails.

## Running a single test

From the `build` directory:

```
ctest --test-dir build -R test_gpu_tools --output-on-failure
```

Or run the executable directly:

```
.\test_gpu_tools.exe    # from build or build/Debug
```

## Test list (examples)

| Executable | Description |
|------------|-------------|
| `test_gpu_tools` | Stubs: IGpuProfiler, IShaderCache, IGpuFence, IShaderSourceCompiler |
| `test_vk_gpu_profiler` | VkGpuProfiler: creation errors (null device/phys/zero ranges), getResult(NotInitialized) |
| `test_vk_gpu_fence` | VkGpuFence: create(null), wait/isSignaled without init |
| `test_vk_gpu_tools_integration` | Integration: real Vulkan (window + device), profiler + fence + command buffer |
| `test_render_correctness` | Render correctness: clear offscreen image to red, readback, check center pixel |
| `test_resize_reveal_cache` | ResizeRevealCache: setShiftStep, pushFrame, frame, variantForSize, empty |
| `test_resize_config` | ResizeMode, ResizeConfig, WindowConfig.resize (defaults and custom) |
| `test_gpu_abi` | Cross-API ABI: PixelFormat, ShaderStage, vertex descriptors, conversion to Vulkan (toVkFormat, etc.) |
| `test_file_utils` | File utilities, including **isPathKeySafe**, **resolveSecure** (path jail) |
| `test_security_layer` | Security layer: isPathKeySafe, resolveSecure (nonexistent path, file under root) |
| `test_shader_cache` | FileShaderCache: unsafe key rejected, path under basePath, SPIR-V limit, setMaxSpirvBytes |
| `test_buffer_manager_integration` | Integration: BufferManager — createBuffer(0 / >256 MiB) error, map() bounds check |
| `test_shader_limits_integration` | Integration: ShaderModule — loadFromMemory(0 / >4 MiB) error, maxSpirvBytes, **LoadShaderOptions** |
| `test_staging_integration` | Integration: StagingManager — uploadBuffer(dstSize), **uploadBuffer(BufferManager&, BufferId)**, **uploadImage/updateImage(ImageManager&, ImageId)** |
| `test_cpu_bridge` | CpuServicesHolder (set/get pool, alloc, profiler), SimpleCpuFrameDriver (beginFrame dt, frameIndex, reset) |
| `test_load_shader_options` | LoadShaderOptions: defaults and custom (allowedRoot, maxSpirvBytes) |

Integration tests **test_vk_gpu_tools_integration**, **test_render_correctness**, **test_buffer_manager_integration**, **test_shader_limits_integration**, **test_staging_integration** require a display (window is created hidden). In a headless environment (CI without GUI) they are skipped with a stderr message and return 0.

## Headless / CI

- In CI (e.g. `.github/workflows/ci.yml`) build and `ctest` run without a display; tests that need a window are skipped and return 0.
- **main_headless** test: `vapi_main --headless` — main app without window; always runs in CI.
- For a full local run with display, run `ctest --test-dir build --output-on-failure`.

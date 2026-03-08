# Core module

Types, errors, logging, file utilities, easing, and math helpers.

- **types.hpp** — Type aliases (u8–u64, s8–s64, f32/f64), `point`, `vec2`–`vec4`, `rect`, `color3`/`color4`, `SizeElement`, `Result<T>`, `Handle<T>`, `Coord`, `Easing`, `LogLevel`, `LogEntry`.
- **error.hpp** — `Error`, `errors::*`, `errors::file::*`, `Result<T>`, `errorMessage(Error)`, `VAPI_TRY` / `VAPI_TRY(var, expr)` (MSVC) / `VAPI_TRY_ASSIGN` / `VAPI_TRY_VOID`.
- **logger.hpp/cpp** — `ConsoleSink`, `FileSink`, `Logger`, log macros.
- **file_utils.hpp/cpp** — Path helpers (`getExtension`, `getStem`, `getParent`), `fileExists`, `fileSize`, `readText`/`writeText`, `readBinary`/`writeBinary`, `tempFilePath`, `FileByteSource`, `FileByteSink`.
- **easing.hpp/cpp** — `Easing` enum, `applyEasing(Easing, f32)`.
- **math_utils.hpp/cpp** — Constants (PI, DEG_TO_RAD), `clamp`, `lerp`, `smoothstep`, `wrap`, `distance`, `rotate`, `hsvToRgb`, rect/vec2/color helpers.
- **obfuscate.hpp** — `ObfString`, `ObfPassthrough`, `VAPI_OBF` / `VAPI_OBF_LIT`.

Interfaces in **interfaces/** (`i_log.hpp`, `i_byte.hpp`, `i_window.hpp` with `IWindowControl`/`IWindow`, `i_input.hpp`, `i_lifecycle.hpp`, `i_gpu.hpp`, `i_resource.hpp`, `i_drawable.hpp`, `i_event.hpp`, `i_updatable.hpp`, `i_manager.hpp`, `i_serializable.hpp`, `i_cpu_tools.hpp`, `i_gpu_tools.hpp`, etc.). Some interfaces are forward-looking (stub or no implementation yet) and may be refined when real backends are added. **tools/** provides stubs (`cpu_tools_stub.hpp`, `gpu_tools_stub.hpp`) и реальные реализации: `cpu_profiler.hpp` (IProfiler), `thread_pool.hpp` (IThreadPool), `host_allocator.hpp` (IHostAllocator), `shader_cache_file.hpp`, `resize_reveal_cache.hpp`; в модуле GPU — VkGpuProfiler, VkGpuFence. Include: `#include "core/…"`. Link: `vapi_core`.

# Core module — Detailed API

Include: `#include "core/types.hpp"`, `"core/error.hpp"`, `"core/logger.hpp"`, `"core/file_utils.hpp"`, `"core/math_utils.hpp"`, `"core/easing.hpp"` or use root `#include "vapi.hpp"` (which pulls in types, error, logger, file_utils).

---

## 1. Type aliases (types.hpp)

| Alias | Meaning |
|-------|---------|
| `u8`–`u64`, `s8`–`s64` | Fixed-width integers |
| `f32`, `f64` | Float types |
| `usize`, `ssize` | Size / pointer-sized |
| `WindowId` | `u32` for window identifier |

Use these instead of raw `uint32_t` etc. for consistency.

---

## 2. Error and Result (error.hpp)

### Error

- **Type:** `struct Error { u32 code; }`.
- **Methods:** `value()`, `ok()`, `operator==`, `operator bool()` (true when error).
- **Constants:** `errors::Ok`, `errors::Init`, `errors::InvalidHandle`, `errors::NotInitialized`, …; `errors::file::NotFound`, `errors::file::OpenFailed`, etc.

```cpp
#include "core/error.hpp"
if (!e.ok())
    std::fprintf(stderr, "Error: %.*s\n", (int)vapi::errorMessage(e).size(), vapi::errorMessage(e).data());
```

### errorMessage(Error e)

- **Signature:** `std::string_view errorMessage(Error e) noexcept`.
- **Returns:** A single human-readable string for the error code (e.g. `"File not found"`, `"Invalid handle"`). Unknown codes yield `"Unknown error"`.

### Result<T>

- **Type:** `Result<T> = std::expected<T, Error>`.
- **Usage:** `has_value()`, `operator*`, `value()`, `error()`, `value_or(default)`.

### Macros

- **VAPI_TRY(expr)** (GCC/Clang): Unwraps `Result`; on error returns from function. Yields the value.
- **VAPI_TRY(var, expr)** (MSVC): Same idea: assigns unwrapped value to `var` or returns error.
- **VAPI_TRY_ASSIGN(var, expr):** Assign result to `var`; on error return from function.
- **VAPI_TRY_VOID(expr):** For `Result<void>`; on error return from function.

```cpp
Result<int> compute() {
    VAPI_TRY_ASSIGN(x, parseNumber());
    return x * 2;
}
```

---

## 3. Handle and HandlePool (types.hpp)

### Handle<T>

- **Fields:** `index`, `generation`.
- **Methods:** `isValid()`, `Handle::null()`.
- **Use:** Opaque handle for resources; invalidated when freed (generation check).

### HandlePool<T>

- **Methods:** `allocate()` → `Handle<T>`, `free(handle)`, `isValid(handle)`, `activeCount()`, `clear()`, `reserve(capacity)`.
- **Use:** Manages a pool of handles with generation to detect use-after-free.

```cpp
vapi::HandlePool<MyResource> pool;
auto h = pool.allocate();
if (pool.isValid(h)) { /* use h */ pool.free(h); }
```

---

## 4. Geometric and color types (types.hpp)

- **point**, **vec2**, **vec3**, **vec4**: `x,y(,z,w)` with arithmetic and helpers (e.g. `dot`, `length` on vec2).
- **rect**: `min`, `max` (point).
- **color3**, **color4**: RGB/RGBA with `r,g,b(,a)`.
- **SizeElement**: `width`, `height` (u32).
- **Coord**, **CoordSystem**, **AxisDirection**: Coordinate system description.
- **Easing**: Enum for easing curves (used with `easing.hpp`).

```cpp
#include "core/types.hpp"
vapi::vec2 a(1.f, 0.f);
vapi::vec2 b = a + vapi::vec2(0.f, 1.f);
vapi::rect r{ {0, 0}, {100, 100} };
vapi::color4 red = vapi::color4::red();
vapi::SizeElement size(800, 600);
```

---

## 5. File utilities (file_utils.hpp)

Namespace: `vapi::file`. Uses `std::filesystem::path` as `fs::path`.

### Queries (noexcept where indicated)

- `fileExists(p)`, `isFile(p)`, `isDirectory(p)`, `isSymlink(p)`, `fileSize(p)`.

### Read/Write (return `Result<...>`)

- `readText(p)`, `readBinary(p)`, `readLines(p)`, `readTextLimited(p, max)`.
- `writeText(p, content, append)`, `writeBinary(p, span, append)`, `writeLines(p, lines, append)`, `appendText(p, content)`.

### Path helpers

- `getExtension(p)`, `getStem(p)`, `getParent(p)`.
- `listFiles(dir, recursive)`, `listFilesByExtension(dir, ext, rec)`.
- `lastModified(p)`, `isNewerThan(a, b)`, `canonicalize(p)`.
- `tempFilePath(prefix, ext)`.

### File operations

- `createDirectories(p)`, `copyFile(from, to, overwrite)`, `removeFile(p)`, `renameFile(from, to)`.

### FileByteSource / FileByteSink

- Implement `IByteSource` / `IByteSink`; `FileByteSource(path)`, `read(max)`; `FileByteSink(path, append)`, `write(span)`, `flush()`.

```cpp
// Read/write text
auto text = vapi::file::readText("config.json");
if (!text) return;
std::string content = *text;

// Path helpers
std::string ext = vapi::file::getExtension(path);
vapi::file::fs::path parent = vapi::file::getParent(path);
auto files = vapi::file::listFilesByExtension("assets", ".png");

// Stream-style byte read/write
vapi::file::FileByteSource src("data.bin");
auto bytes = src.read();
vapi::file::FileByteSink sink("out.bin");
if (bytes) sink.write(*bytes);
sink.flush();
```

---

## 6. Logger (logger.hpp)

### Logger::instance()

- **Singleton.** Methods: `setMinLevel(level)`, `setMinLevel(category, level)`, `addSink(unique_ptr<ILogSink>)`, `clearSinks()`, `log(level, category, message, source_location)`, `shouldLog(level, category)`.

### Sinks

- **ConsoleSink:** Writes to console; `setColorEnabled(bool)`.
- **FileSink:** Writes to file; constructor takes file path.

### Free functions

- `logDebug(category, message)`, `logInfo(...)`, `logWarn(...)`, `logError(...)`.
- **LogLevel:** `Debug`, `Info`, `Warning`, `Error`.
- **LogEntry:** Contains level, category, message, timestamp, source location; when **VAPI_LOG_DETAIL** is defined also `deviceName` (hostname + optional MAC) and `callAddress` (call-site execution address).

### VAPI_LOG_DETAIL

When **VAPI_LOG_DETAIL** is defined (e.g. in build system or before including the logger):

- Every log line includes **time**, **device name** (hostname; plus MAC on Windows/Linux when available), **entry point** (file:line and function name), and **execution address** (call-site pointer in hex).
- The macros `logDebug`, `logInfo`, `logWarn`, `logError`, `logFatal` pass the call-site address so the logger can print it; without the define, behaviour is unchanged (time, level, category, message; location only for Error and above).

```cpp
// Basic usage (no VAPI_LOG_DETAIL)
vapi::logInfo("App", "Started");
vapi::logError("GPU", "Device lost");

// Add file sink
vapi::Logger::instance().addSink(std::make_unique<vapi::FileSink>("app.log"));

// Category-specific level
vapi::Logger::instance().setMinLevel("Noise", vapi::LogLevel::Warning);

// With VAPI_LOG_DETAIL (define before including logger, or -DVAPI_LOG_DETAIL):
// #define VAPI_LOG_DETAIL
// #include "core/logger.hpp"
// logInfo("App", "Started");
// Output includes: [HH:MM:SS.mmm] [INF] [App] Started [device:MYPC MAC:00:11:22:33:44:55] (file.cpp:42 function_name) [addr:0x7ff6a1b2]
```

---

## 7. Math utilities (math_utils.hpp)

Namespace: `vapi::math`.

### Constants

- `PI`, `TWO_PI`, `HALF_PI`, `DEG_TO_RAD`, `RAD_TO_DEG`, `EPSILON`.

### Functions

- `clamp(value, lo, hi)`, `lerp(a, b, t)`, `inverseLerp(a, b, v)`, `mapRange(v, inMin, inMax, outMin, outMax)`.
- `degToRad(d)`, `radToDeg(r)`.
- `smoothstep(e0, e1, x)`, `smootherstep(e0, e1, x)`.
- `sign(v)`, `approxEqual(a, b, eps)`.
- `wrap(v, max)`, `wrap(v, min, max)`.
- `distance(a, b)`, `distanceSq(a, b)` for points; `lerp`, `clamp`, `reflect`, `rotate`, `angle` for vec2.
- `lerp`, `intersection`, `unionRect` for rects.
- `hsvToRgb(h, s, v)` (in math_utils or types).

```cpp
float t = vapi::math::smoothstep(0.f, 1.f, x);
vapi::vec2 p = vapi::math::lerp(a, b, t);
```

---

## 8. Easing (easing.hpp)

- **Easing enum:** `Linear`, `EaseInQuad`, `EaseOutQuad`, `EaseInOutQuad`, cubic, quart, quint, sine, expo, circ, back, elastic, bounce variants.
- **applyEasing(Easing e, f32 t):** Returns eased value in [0,1] for `t` in [0,1].

```cpp
float eased = vapi::applyEasing(vapi::Easing::EaseOutCubic, t);
```

---

## 9. Core interfaces (core/interfaces/)

Summarized; see headers for full contracts.

- **IByteSource / IByteSink:** `read()` / `write(span)`, `flush()`.
- **ILogSink:** `write(LogEntry)`, `flush()`.
- **IWindowControl:** All window control/query (show, hide, size, position, focus, title, close, state).
- **IWindow:** Extends IWindowControl; adds createWindow, destroyWindow, setCallbacks, getNativeHandle.
- **IInput:** Keyboard, mouse, gamepad, poll; setInputCallbacks.
- **IInitializable:** `init()`, `isInitialized()`, `terminate()`.
- **ICreatable<Handle,Desc>:** `create(desc)`.
- **IDestroyable<Handle>:** `destroy(handle)`, `destroyAll()`.
- **IValidatable<Handle>:** `isValid(handle)`.
- **i_gpu.hpp:** Device properties, queue families, swapchain support (see gpu.md).
- **i_resource.hpp:** Memory usage, buffer/image usage enums.
- **i_cpu_tools.hpp:** IProfiler, IThreadPool, IHostAllocator. Stubs in core/tools/; real implementations (ThreadPool, HostAllocatorAligned) in core/tools/.
- **i_cpu_bridge.hpp:** Interfaces for linking the application (CPU) with VAPI: **ICpuServices** (facade: getThreadPool(), getHostAllocator(), getProfiler()); **IAppLogic** (update(dt), onBeginFrame(), onEndFrame()) — implement in the app, call from main loop; **IAppFixedTick** (extends IAppLogic, adds fixedUpdate(fixedDt)); **ICpuFrameDriver** (beginFrame() → dt, endFrame(), frameIndex()). Implementations: **core/tools/cpu_bridge_impl.hpp** — **CpuServicesHolder** (non-owning holder for pool/alloc/profiler), **SimpleCpuFrameDriver** (chrono-based dt).
- **i_gpu_tools.hpp:** IGpuProfiler, IShaderCache, IGpuFence, IShaderSourceCompiler. Stubs in core/tools/; Vulkan implementations (VkGpuProfiler, VkGpuFence) in the gpu module (see gpu.md §10). Pass the current command buffer from the record site as `void*` when using the Vulkan profiler.
- **i_gpu_abi.hpp:** Cross-API GPU types (no Vulkan/D3D in the header): **PixelFormat**, **ShaderStage**, **ShaderStageFlags**, **VertexInputRate**, **VertexAttributeDesc**, **VertexBindingDesc**, **PrimitiveTopology**, **CompareOp**. Use these to describe pipelines and resources in a backend-agnostic way. The gpu module provides **vk_abi_convert.hpp** with `toVkFormat()`, `toVkShaderStage()`, `toVkVertexInputAttributeDescription()`, etc., to convert to Vulkan; future D3D/Metal backends would provide their own conversion.

Use these for abstraction and testing; platform and GPU modules implement them.

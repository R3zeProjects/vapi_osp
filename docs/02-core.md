# Core

Types, logger, errors, and utilities. Headers: core/types.hpp, core/logger.hpp, core/error.hpp, core/file_utils.hpp, core/math_utils.hpp, core/string_utils.hpp, core/obfuscate.hpp.

## Types (core/types.hpp)

Integer: u8, u16, u32, u64, s8, s16, s32, s64, usize, sptr, uptr. Floating: f32, f64. Geometry and color: vec2, Rect (x, y, w, h), color4, Padding, Margin, Resolution. SizePolicy — size policy (fixed, fill, percent, fit) for widgets.

## Logger (core/logger.hpp)

Levels: Debug, Info, Warning, Error, Fatal, None. **Logger::instance()** — singleton. **setMinLevel(level)** / **setMinLevel(category, level)**. **addSink(unique_ptr<ILogSink>)**. **log(level, category, message [, location])**. Sinks: ConsoleSink, FileSink(filepath). Helpers: logDebug, logInfo, logWarning, logError, logFatal.

## Errors (core/error.hpp)

**Result<T>** — operation result or error. **errors::** — error codes. **file_errors::** — file errors. Check: `if (!r) return r.error();`

## Interfaces (core/interfaces/)

IWindow, IInput, IResource, IDrawable, ILifecycle, IUpdatable, IEvent, IManager, IRenderer, etc. — contracts; implementations in platform, ui, render.

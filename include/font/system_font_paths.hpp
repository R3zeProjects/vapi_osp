/** @file system_font_paths.hpp
 *  @brief System font directories and default font path per OS.
 *  Vector fonts: .ttf/.otf supported by STB and most backends. */

#ifndef VAPI_FONT_SYSTEM_FONT_PATHS_HPP
#define VAPI_FONT_SYSTEM_FONT_PATHS_HPP

#include "core/types.hpp"
#include <string>
#include <vector>

namespace vapi {

/// Collect system font directories (where to search for .ttf/.otf).
[[nodiscard]] std::vector<std::string> getSystemFontDirectories();

/// Preferred default font path for this OS (e.g. Arial on Windows, Liberation Sans on Linux).
[[nodiscard]] std::string getDefaultSystemFontPath();

/// Resolve font by name: search system dirs for a file matching family name or filename.
[[nodiscard]] std::string resolveSystemFontPath(std::string_view fontNameOrPath);

/// Paths to try for loading a system TTF (e.g. for fallback when default fails). Order: default first, then by name.
[[nodiscard]] std::vector<std::string> getSystemFontPathsToTry();

} // namespace vapi

#endif // VAPI_FONT_SYSTEM_FONT_PATHS_HPP

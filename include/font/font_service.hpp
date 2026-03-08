/** @file font_service.hpp
 *  @brief Load fonts from path or system default; manage font instances.
 *  Replaceable: swap implementation to use different backends. */

#ifndef VAPI_FONT_FONT_SERVICE_HPP
#define VAPI_FONT_FONT_SERVICE_HPP

#include "font/font_types.hpp"
#include "font/i_font_source.hpp"
#include "font/stb_font_source.hpp"
#include "core/error.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vapi {

/// Loads and caches fonts by path or name. Uses system font paths when path not absolute.
class FontService {
public:
    FontService() = default;

    /// Load font from path (or resolve via resolveSystemFontPath if not absolute).
    [[nodiscard]] Result<FontId> loadFont(std::string_view pathOrName);

    /// Get font source for drawing. Returns nullptr if id invalid.
    [[nodiscard]] IFontSource* getFont(FontId id);
    [[nodiscard]] const IFontSource* getFont(FontId id) const;

    void unloadFont(FontId id);
    void unloadAll();

private:
    FontId m_nextId{1u};
    std::unordered_map<FontId, std::unique_ptr<StbFontSource>> m_fonts;
};

} // namespace vapi

#endif // VAPI_FONT_FONT_SERVICE_HPP

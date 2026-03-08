/** @file resize_reveal_cache.hpp
 *  @brief Cache of pre-rendered frames with pixel shift for ResizeMode::RevealHidden.
 *  Builds frames with shift 0, m, 2m, 3m so when the user stretches the window
 *  a picture appears immediately instead of bands. */

#ifndef VAPI_CORE_TOOLS_RESIZE_REVEAL_CACHE_HPP
#define VAPI_CORE_TOOLS_RESIZE_REVEAL_CACHE_HPP

#include "core/types.hpp"
#include <vector>
#include <cstring>

namespace vapi::tools {

/** Cache of one source frame and up to 4 variants: original + shifted by 0, m, 2m, 3m pixels (horizontal). */
class ResizeRevealCache {
public:
    ResizeRevealCache() = default;

    /** Shift step in pixels (1..3). Frames are built with shift 0, m, 2m, 3m. */
    void setShiftStep(u8 m) {
        m_shiftStep = (m < 1) ? 1u : (m > 3 ? 3u : static_cast<u32>(m));
    }
    [[nodiscard]] u32 shiftStep() const { return m_shiftStep; }

    /** Push current frame (RGBA, row-major). Builds internal copies with shifts 0, m, 2m, 3m. */
    void pushFrame(const u8* pixels, u32 width, u32 height, u32 bytesPerPixel = 4) {
        if (!pixels || width == 0 || height == 0) return;
        const u32 stride = width * bytesPerPixel;
        const usize size = static_cast<usize>(height) * stride;
        m_width = width;
        m_height = height;
        m_bytesPerPixel = bytesPerPixel;

        m_frames.resize(4);
        for (auto& f : m_frames) f.resize(size);

        // Frame 0: original
        std::memcpy(m_frames[0].data(), pixels, size);

        // Frames 1,2,3: shifted by m, 2m, 3m pixels (horizontal) — content shifted left, right edge zeroed
        const u32 rowBytes = width * bytesPerPixel;
        for (u32 k = 1; k <= 3; ++k) {
            const u32 shift = m_shiftStep * k;
            if (shift >= width) {
                std::memcpy(m_frames[k].data(), m_frames[0].data(), size);
                continue;
            }
            u8* dst = m_frames[k].data();
            const u32 shiftBytes = shift * bytesPerPixel;
            const u32 copyBytes = rowBytes - shiftBytes;
            for (u32 y = 0; y < height; ++y) {
                const u8* rowSrc = m_frames[0].data() + static_cast<usize>(y) * rowBytes;
                std::memcpy(dst, rowSrc + shiftBytes, copyBytes);
                std::memset(dst + copyBytes, 0, shiftBytes);
                dst += rowBytes;
            }
        }
    }

    /** Number of cached variants (1 = original only, 4 = original + 3 shifted). */
    [[nodiscard]] u32 variantCount() const { return 4u; }

    /** Get pointer to variant index (0..3). Returns nullptr if no frame pushed yet. */
    [[nodiscard]] const u8* frame(u32 variantIndex) const {
        if (variantIndex >= m_frames.size() || m_frames[variantIndex].empty()) return nullptr;
        return m_frames[variantIndex].data();
    }

    /** Suggest variant for new size: минимизация видимых полос по горизонтали и учёт вертикали. Returns 0..3. */
    [[nodiscard]] u32 variantForSize(u32 newWidth, u32 newHeight) const {
        if (m_width == 0 || m_height == 0) return 0;
        const u32 step = m_shiftStep > 0 ? m_shiftStep : 1u;
        u32 dw = (newWidth >= m_width) ? (newWidth - m_width) : 0;
        u32 dh = (newHeight >= m_height) ? (newHeight - m_height) : 0;
        // Учёт обеих размерностей для выбора варианта сдвига (0..3)
        u32 slot = ((dw / step) + (dh / step)) % 4u;
        return slot;
    }

    [[nodiscard]] u32 width() const { return m_width; }
    [[nodiscard]] u32 height() const { return m_height; }
    [[nodiscard]] u32 bytesPerPixel() const { return m_bytesPerPixel; }
    [[nodiscard]] bool empty() const { return m_frames.empty() || m_frames[0].empty(); }

private:
    u32 m_shiftStep{1};
    u32 m_width{0};
    u32 m_height{0};
    u32 m_bytesPerPixel{4};
    std::vector<std::vector<u8>> m_frames;
};

} // namespace vapi::tools

#endif // VAPI_CORE_TOOLS_RESIZE_REVEAL_CACHE_HPP

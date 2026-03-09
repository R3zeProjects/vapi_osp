#ifndef VAPI_RESOURCE_TYPES_HPP
#define VAPI_RESOURCE_TYPES_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include "core/interfaces/i_resource.hpp"
#include <vulkan/vulkan.h>
#include <functional>
#include <vector>
namespace vapi {

// ── Strong-typed resource IDs ────────────────────────────────
enum class BufferId     : u32 {};
enum class ImageId      : u32 {};
enum class SamplerId    : u32 {};
enum class DescLayoutId : u32 {};
enum class DescSetId    : u32 {};

inline constexpr BufferId     kNullBufferId{0};
inline constexpr ImageId      kNullImageId{0};
inline constexpr SamplerId    kNullSamplerId{0};
inline constexpr DescLayoutId kNullDescLayoutId{0};
inline constexpr DescSetId    kNullDescSetId{0};

inline constexpr BufferId     kInvalidBufferId{UINT32_MAX};
inline constexpr ImageId      kInvalidImageId{UINT32_MAX};
inline constexpr SamplerId    kInvalidSamplerId{UINT32_MAX};
inline constexpr DescLayoutId kInvalidDescLayoutId{UINT32_MAX};
inline constexpr DescSetId    kInvalidDescSetId{UINT32_MAX};

template<typename IdT>
constexpr u32 toIndex(IdT id) noexcept { return static_cast<u32>(id); }

template<typename IdT>
IdT advanceId(IdT& id) noexcept {
    auto cur = id;
    id = static_cast<IdT>(static_cast<u32>(id) + 1);
    return cur;
}

// ── Buffer ────────────────────────────────────────────────────

struct GpuBuffer {
    VkBuffer       buffer{VK_NULL_HANDLE};
    VkDeviceMemory memory{VK_NULL_HANDLE};
    VkDeviceSize   size{0};
    VkBufferUsageFlags usage{0};
    MemoryUsage    memoryUsage{MemoryUsage::GpuOnly};
    void*          mapped{nullptr};  // persistent map (for CpuToGpu)
    bool           valid{false};
};

// ── Image ─────────────────────────────────────────────────────

enum class ImageType : u8 { Texture2D, DepthStencil, RenderTarget, Storage };

struct ImageDesc {
    u32 width{1};
    u32 height{1};
    u32 mipLevels{1};
    u32 arrayLayers{1};
    VkFormat format{VK_FORMAT_R8G8B8A8_SRGB};
    VkImageUsageFlags usage{VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT};
    ImageType type{ImageType::Texture2D};
    MemoryUsage memoryUsage{MemoryUsage::GpuOnly};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
};

struct GpuImage {
    VkImage        image{VK_NULL_HANDLE};
    VkDeviceMemory memory{VK_NULL_HANDLE};
    VkImageView    view{VK_NULL_HANDLE};
    ImageDesc      desc;
    VkImageLayout  currentLayout{VK_IMAGE_LAYOUT_UNDEFINED};
    bool           valid{false};
};

// ── Sampler ───────────────────────────────────────────────────

enum class FilterMode : u8 { Nearest, Linear };
enum class WrapMode   : u8 { Repeat, MirroredRepeat, ClampToEdge, ClampToBorder };

struct SamplerDesc {
    FilterMode magFilter{FilterMode::Linear};
    FilterMode minFilter{FilterMode::Linear};
    FilterMode mipFilter{FilterMode::Linear};
    WrapMode   wrapU{WrapMode::Repeat};
    WrapMode   wrapV{WrapMode::Repeat};
    WrapMode   wrapW{WrapMode::Repeat};
    f32        maxAnisotropy{16.f};
    bool       anisotropyEnable{true};
    f32        minLod{0.f};
    f32        maxLod{VK_LOD_CLAMP_NONE};
    f32        mipLodBias{0.f};

    bool operator==(const SamplerDesc&) const = default;

    /** Сэмплер для атласа шрифтов: интерполяция ближайшего соседа (при масштабировании — чёткие пиксели). */
    static constexpr SamplerDesc fontAtlas() {
        SamplerDesc d{};
        d.magFilter = d.minFilter = d.mipFilter = FilterMode::Nearest;
        d.wrapU = d.wrapV = d.wrapW = WrapMode::ClampToEdge;
        d.anisotropyEnable = false;
        return d;
    }

    /** Сэмплер для атласа шрифтов со сглаживанием: линейная интерполяция для anti-aliasing. */
    static constexpr SamplerDesc fontAtlasSmooth() {
        SamplerDesc d{};
        d.magFilter = d.minFilter = FilterMode::Linear;
        d.mipFilter = FilterMode::Nearest;
        d.wrapU = d.wrapV = d.wrapW = WrapMode::ClampToEdge;
        d.anisotropyEnable = false;
        return d;
    }
};

struct GpuSampler {
    VkSampler   sampler{VK_NULL_HANDLE};
    SamplerDesc desc;
    bool        valid{false};
};

// ── Descriptor ────────────────────────────────────────────────

struct DescriptorBinding {
    u32 binding{0};
    VkDescriptorType type{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
    u32 count{1};
    VkShaderStageFlags stageFlags{VK_SHADER_STAGE_ALL};
};

struct DescriptorLayoutDesc {
    std::vector<DescriptorBinding> bindings;
};

struct DescriptorPoolSize {
    VkDescriptorType type;
    u32 count;
};

/// Bytes per pixel for R8G8B8A8 format (default for createTexture/updateTexture).
inline constexpr u32 kTextureBytesPerPixelRGBA = 4u;

} // namespace vapi

// ── std::hash specializations for strong-typed IDs ───────────
template<> struct std::hash<vapi::BufferId> {
    std::size_t operator()(vapi::BufferId id) const noexcept { return std::hash<vapi::u32>{}(static_cast<vapi::u32>(id)); }
};
template<> struct std::hash<vapi::ImageId> {
    std::size_t operator()(vapi::ImageId id) const noexcept { return std::hash<vapi::u32>{}(static_cast<vapi::u32>(id)); }
};
template<> struct std::hash<vapi::SamplerId> {
    std::size_t operator()(vapi::SamplerId id) const noexcept { return std::hash<vapi::u32>{}(static_cast<vapi::u32>(id)); }
};
template<> struct std::hash<vapi::DescLayoutId> {
    std::size_t operator()(vapi::DescLayoutId id) const noexcept { return std::hash<vapi::u32>{}(static_cast<vapi::u32>(id)); }
};
template<> struct std::hash<vapi::DescSetId> {
    std::size_t operator()(vapi::DescSetId id) const noexcept { return std::hash<vapi::u32>{}(static_cast<vapi::u32>(id)); }
};

#endif

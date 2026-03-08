#ifndef VAPI_CORE_I_RESOURCE_HPP
#define VAPI_CORE_I_RESOURCE_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <span>

namespace vapi {

enum class MemoryUsage : u8 { GpuOnly, CpuOnly, CpuToGpu, GpuToCpu };
enum class BufferUsage : u32 {
    None = 0, Vertex = 1<<0, Index = 1<<1, Uniform = 1<<2, Storage = 1<<3,
    Indirect = 1<<4, TransferSrc = 1<<5, TransferDst = 1<<6
};
inline BufferUsage operator|(BufferUsage a, BufferUsage b) {
    return static_cast<BufferUsage>(static_cast<u32>(a) | static_cast<u32>(b));
}
inline bool operator&(BufferUsage a, BufferUsage b) {
    return (static_cast<u32>(a) & static_cast<u32>(b)) != 0;
}

struct BufferDesc {
    usize size{0}; BufferUsage usage{BufferUsage::None};
    MemoryUsage memoryUsage{MemoryUsage::GpuOnly};
};
struct MemoryRequirements { usize size{0}; usize alignment{0}; u32 memoryTypeBits{0}; };

template<typename Handle>
class IMappable {
public:
    virtual ~IMappable() = default;
    [[nodiscard]] virtual Result<void*> map(Handle h, usize offset = 0, usize size = 0) = 0;
    virtual void unmap(Handle h) = 0;
    [[nodiscard]] virtual bool isMapped(Handle h) const = 0;
};
template<typename Handle>
class IWritable {
public:
    virtual ~IWritable() = default;
    [[nodiscard]] virtual Result<void> write(Handle h, std::span<const u8> data, usize offset = 0) = 0;
};
template<typename Handle>
class IReadable {
public:
    virtual ~IReadable() = default;
    [[nodiscard]] virtual Result<usize> read(Handle h, std::span<u8> buffer, usize offset = 0) = 0;
};
template<typename Handle>
class ICopyable {
public:
    virtual ~ICopyable() = default;
    [[nodiscard]] virtual Result<void> copy(Handle src, Handle dst, usize srcOff = 0, usize dstOff = 0, usize size = 0) = 0;
};
template<typename Handle>
class IFlushable {
public:
    virtual ~IFlushable() = default;
    [[nodiscard]] virtual Result<void> flush(Handle h, usize offset = 0, usize size = 0) = 0;
    [[nodiscard]] virtual Result<void> invalidate(Handle h, usize offset = 0, usize size = 0) = 0;
};
template<typename ResourceHandle, typename MemoryHandle>
class IBindable {
public:
    virtual ~IBindable() = default;
    [[nodiscard]] virtual Result<void> bind(ResourceHandle res, MemoryHandle mem, usize offset = 0) = 0;
    [[nodiscard]] virtual MemoryRequirements getMemoryRequirements(ResourceHandle res) const = 0;
};
template<typename AllocResult>
class IAllocator {
public:
    virtual ~IAllocator() = default;
    [[nodiscard]] virtual Result<AllocResult> allocate(usize size, usize alignment = 1) = 0;
    virtual void free(AllocResult& alloc) = 0;
    virtual void free_all() = 0;
};

} // namespace vapi

#endif

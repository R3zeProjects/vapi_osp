/** @file host_allocator.hpp
 *  @brief Real host allocator implementation (IHostAllocator): aligned allocation. */

#ifndef VAPI_CORE_TOOLS_HOST_ALLOCATOR_HPP
#define VAPI_CORE_TOOLS_HOST_ALLOCATOR_HPP

#include "core/interfaces/i_cpu_tools.hpp"
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
# include <malloc.h>
#endif

namespace vapi::tools {

/** Host allocator with proper alignment support (for GPU upload buffers, etc.). */
class HostAllocatorAligned : public IHostAllocator {
public:
    [[nodiscard]] Result<void*> allocate(usize size, usize alignment = 1) override {
        if (size == 0) return nullptr;
        if (alignment < 1) alignment = 1;
        void* p = nullptr;
#if defined(_WIN32) || defined(_WIN64)
        p = _aligned_malloc(size, alignment);
#else
        if (alignment <= alignof(std::max_align_t)) {
            p = std::malloc(size);
        } else {
            usize a = alignment;
            if ((a & (a - 1)) != 0) { /* round up to power of two */ a |= a >> 1; a |= a >> 2; a |= a >> 4; a++; }
            if (posix_memalign(&p, a, size) != 0) p = nullptr;
        }
#endif
        if (!p) return std::unexpected(errors::OutOfMemory);
        return p;
    }
    void deallocate(void* ptr) override {
        if (!ptr) return;
#if defined(_WIN32) || defined(_WIN64)
        _aligned_free(ptr);
#else
        std::free(ptr);
#endif
    }
    [[nodiscard]] bool owns(void* /*ptr*/) const override { return true; }
};

} // namespace vapi::tools

#endif // VAPI_CORE_TOOLS_HOST_ALLOCATOR_HPP

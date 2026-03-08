#ifndef VAPI_CORE_I_BYTE_HPP
#define VAPI_CORE_I_BYTE_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <span>
#include <vector>
#include <limits>

namespace vapi {

inline constexpr usize kReadAll = std::numeric_limits<usize>::max();

class IByteSource {
public:
    virtual ~IByteSource() = default;
    [[nodiscard]] virtual Result<std::vector<u8>> read(usize max = kReadAll) = 0;
    [[nodiscard]] Result<std::vector<u8>> readAll() { return read(kReadAll); }
};

class IByteSink {
public:
    virtual ~IByteSink() = default;
    [[nodiscard]] virtual Result<void> write(std::span<const u8> data) = 0;
    [[nodiscard]] virtual Result<void> flush() = 0;
};

class IByteTransform {
public:
    virtual ~IByteTransform() = default;
    [[nodiscard]] virtual Result<std::vector<u8>> transform(std::span<const u8> in) = 0;
};

class ChainedByteSource : public IByteSource {
public:
    ChainedByteSource(IByteSource& source, std::span<IByteTransform* const> transforms)
        : source_(source), transforms_(transforms.begin(), transforms.end()) {}
    [[nodiscard]] Result<std::vector<u8>> read(usize max = kReadAll) override {
        auto data = source_.read(max);
        if (!data) return std::unexpected(data.error());
        for (IByteTransform* t : transforms_) {
            auto next = t->transform(*data);
            if (!next) return std::unexpected(next.error());
            data = std::move(*next);
        }
        return data;
    }
private:
    IByteSource& source_;
    std::vector<IByteTransform*> transforms_;
};

class ChainedByteSink : public IByteSink {
public:
    ChainedByteSink(IByteSink& sink, std::span<IByteTransform* const> transforms)
        : sink_(sink), transforms_(transforms.begin(), transforms.end()) {}
    [[nodiscard]] Result<void> write(std::span<const u8> data) override {
        std::vector<u8> buf(data.begin(), data.end());
        for (auto it = transforms_.rbegin(); it != transforms_.rend(); ++it) {
            auto next = (*it)->transform(buf);
            if (!next) return std::unexpected(next.error());
            buf = std::move(*next);
        }
        return sink_.write(buf);
    }
    [[nodiscard]] Result<void> flush() override { return sink_.flush(); }
private:
    IByteSink& sink_;
    std::vector<IByteTransform*> transforms_;
};

} // namespace vapi

#endif

/** @file asset_loader.hpp
 *  @brief Async asset loading interface and simple implementation.
 *  Callback may be invoked from a worker thread; marshal to main thread if needed. */

#ifndef VAPI_CORE_ASSET_LOADER_HPP
#define VAPI_CORE_ASSET_LOADER_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <memory>

namespace vapi {

/** Result of loading a binary asset (file contents). */
using AssetResult = Result<std::vector<u8>>;

/** Callback for async load. May be called from a worker thread. */
using AssetLoadCallback = std::function<void(AssetResult)>;

/**
 * Interface for async asset loading (e.g. files, resources).
 * loadAsync() starts a load; the callback is invoked when done (possibly from another thread).
 */
class IAssetLoader {
public:
    virtual ~IAssetLoader() = default;
    /** Start loading path; callback is invoked with result (may be from worker thread). */
    virtual void loadAsync(std::string_view path, AssetLoadCallback callback) = 0;
};

/** Create a simple loader that reads the file on a std::thread and invokes callback with the result. */
[[nodiscard]] std::unique_ptr<IAssetLoader> createSimpleAssetLoader();

} // namespace vapi

#endif // VAPI_CORE_ASSET_LOADER_HPP

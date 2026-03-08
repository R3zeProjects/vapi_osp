/** @file thread_pool.hpp
 *  @brief Real thread pool implementation (IThreadPool): fixed worker threads, task queue, submit/waitIdle. */

#ifndef VAPI_CORE_TOOLS_THREAD_POOL_HPP
#define VAPI_CORE_TOOLS_THREAD_POOL_HPP

#include "core/interfaces/i_cpu_tools.hpp"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace vapi::tools {

/** Thread pool with a fixed number of worker threads. Tasks are executed in submission order per thread. */
class ThreadPool : public IThreadPool {
public:
    explicit ThreadPool(u32 numThreads = 0);
    ~ThreadPool() override;

    void submit(std::function<void()> task) override;
    void waitIdle() override;
    [[nodiscard]] u32 threadCount() const override { return m_numThreads; }

private:
    void worker();

    u32 m_numThreads{0};
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::condition_variable m_idleCv;
    std::atomic<bool> m_stop{false};
    std::atomic<u32> m_pending{0};
};

} // namespace vapi::tools

#endif // VAPI_CORE_TOOLS_THREAD_POOL_HPP

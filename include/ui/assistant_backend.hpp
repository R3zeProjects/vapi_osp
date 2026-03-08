/** @file assistant_backend.hpp
 *  @brief Assistant (AI/neural) backend interface: connect neural network to the app.
 *  Original API: you implement this to plug in your LLM/API. */

#ifndef VAPI_UI_ASSISTANT_BACKEND_HPP
#define VAPI_UI_ASSISTANT_BACKEND_HPP

#include "core/types.hpp"
#include "core/error.hpp"
#include <string>
#include <functional>
#include <string_view>

namespace vapi::ui {

/// Response from the assistant (streaming or final).
struct AssistantResponse {
    std::string text;
    bool done{false};
    Error error;
};

/// Async callback: called for each chunk or at end. Pass response and whether more will follow.
using AssistantReplyFn = std::function<void(const AssistantResponse&)>;

/** Backend for the Assistant: submit prompt + context, get reply via callback.
 *  Implement this to connect OpenAI-compatible API, local model, or custom service. */
class IAssistantBackend {
public:
    virtual ~IAssistantBackend() = default;

    /// Submit a prompt; optional system context. Reply via callback (may be called multiple times if streaming).
    virtual void submit(std::string_view prompt, std::string_view systemContext,
                        AssistantReplyFn reply) = 0;

    /// Cancel any in-flight request (optional).
    virtual void cancel() = 0;

    /// Whether the backend is ready (e.g. API key set).
    [[nodiscard]] virtual bool isReady() const { return true; }
};

/// Create a simple echo backend (for tests). Replace with your LLM/API backend.
IAssistantBackend* createEchoBackend();

} // namespace vapi::ui

#endif // VAPI_UI_ASSISTANT_BACKEND_HPP

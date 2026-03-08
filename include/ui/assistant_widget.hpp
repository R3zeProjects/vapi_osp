/** @file assistant_widget.hpp
 *  @brief Assistant UI: input, send, response area; binds to IAssistantBackend. */

#ifndef VAPI_UI_ASSISTANT_WIDGET_HPP
#define VAPI_UI_ASSISTANT_WIDGET_HPP

#include "ui/widget.hpp"
#include "ui/assistant_backend.hpp"
#include "ui/layout.hpp"
#include "core/types.hpp"
#include "core/types.hpp"
#include <memory>
#include <string>

namespace vapi::ui {

class IAssistantBackend;

class AssistantWidget : public Widget {
public:
    explicit AssistantWidget(Object* parent = nullptr);
    void setBackend(IAssistantBackend* backend) { m_backend = backend; }
    [[nodiscard]] IAssistantBackend* backend() const { return m_backend; }
    void setSystemContext(const std::string& s) { m_systemContext = s; }
    [[nodiscard]] const std::string& systemContext() const { return m_systemContext; }
    /// Emitted when a response chunk arrives (text, done).
    Emitter<const std::string&, bool>& responseReceived() { return m_responseReceived; }
protected:
    void paint(const DrawContext& ctx) override;
private:
    IAssistantBackend* m_backend{nullptr};
    std::string m_systemContext;
    std::string m_inputBuffer;
    std::string m_responseBuffer;
    bool m_loading{false};
    Emitter<const std::string&, bool> m_responseReceived;
    void onSendClicked();
    void onReply(const AssistantResponse& r);
};

} // namespace vapi::ui

#endif // VAPI_UI_ASSISTANT_WIDGET_HPP

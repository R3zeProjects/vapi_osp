/** @file emitter.hpp
 *  @brief Type-safe event emitter: bind reactions (callables), emit with args.
 *  Original implementation — not derived from any third-party API. */

#ifndef VAPI_UI_EMITTER_HPP
#define VAPI_UI_EMITTER_HPP

#include "core/types.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <cstddef>

namespace vapi::ui {

/// Opaque connection handle: use to disconnect a reaction from an emitter.
class Connection {
public:
    Connection() = default;
    Connection(std::nullptr_t) : m_id(0) {}
    void disconnect();
    [[nodiscard]] bool isConnected() const noexcept { return !m_owner.expired() && m_id != 0; }
    void reset() { m_owner.reset(); m_id = 0; }

private:
    template<typename... Args> friend class Emitter;
    class OwnerBase {
    public:
        virtual ~OwnerBase() = default;
        virtual bool disconnectById(u64 id) = 0;
    };
    std::weak_ptr<OwnerBase> m_owner;
    u64 m_id{0};
    Connection(std::weak_ptr<OwnerBase> owner, u64 id) : m_owner(owner), m_id(id) {}
};

/// Type-safe emitter: reactions are invoked when emit(...) is called.
template<typename... Args>
class Emitter {
public:
    using Reaction = std::function<void(Args...)>;

    Emitter() = default;
    ~Emitter() { m_connections.clear(); }

    Emitter(const Emitter&) = delete;
    Emitter& operator=(const Emitter&) = delete;
    Emitter(Emitter&&) noexcept = default;
    Emitter& operator=(Emitter&&) noexcept = default;

    /// Bind a reaction; returns a connection that can be used to unbind.
    Connection bind(Reaction r) {
        fixOwner();
        if (!r) return Connection(nullptr);
        u64 id = ++m_nextId;
        m_connections.push_back({id, std::move(r)});
        return Connection(m_owner, id);
    }

    /// Emit: invoke all bound reactions with the given arguments.
    /// Snapshots the full reaction list so disconnects during a callback don't skip remaining reactions.
    void emit(Args... args) const {
        auto snapshot = m_connections;
        for (const auto& [id, reaction] : snapshot) {
            if (reaction) reaction(args...);
        }
    }

    [[nodiscard]] std::size_t reactionCount() const noexcept { return m_connections.size(); }

private:
    struct Owner final : Connection::OwnerBase, std::enable_shared_from_this<Owner> {
        Emitter* parent{nullptr};
        bool disconnectById(u64 id) override {
            if (!parent) return false;
            auto& v = parent->m_connections;
            auto it = std::find_if(v.begin(), v.end(), [id](const auto& p) { return p.first == id; });
            if (it == v.end()) return false;
            v.erase(it);
            return true;
        }
    };
    mutable std::shared_ptr<Owner> m_owner;
    u64 m_nextId{0};
    std::vector<std::pair<u64, Reaction>> m_connections;

    void fixOwner() const {
        if (!m_owner) m_owner = std::make_shared<Owner>();
        m_owner->parent = const_cast<Emitter*>(this);
    }
};

inline void Connection::disconnect() {
    if (auto o = m_owner.lock()) {
        if (m_id != 0) o->disconnectById(m_id);
    }
    m_owner.reset();
    m_id = 0;
}

} // namespace vapi::ui

#endif // VAPI_UI_EMITTER_HPP

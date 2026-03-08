/** @file emitter.hpp
 *  @brief Type-safe event emitter: bind reactions (callables), emit with args.
 *  Original implementation — not derived from any third-party API. */

#ifndef VAPI_UI_EMITTER_HPP
#define VAPI_UI_EMITTER_HPP

#include "core/types.hpp"
#include <algorithm>
#include <functional>
#include <vector>
#include <cstddef>

namespace vapi::ui {

/// Opaque connection handle: use to disconnect a reaction from an emitter.
class Connection {
public:
    Connection() = default;
    Connection(std::nullptr_t) : m_owner(nullptr), m_id(0) {}
    void disconnect();
    [[nodiscard]] bool isConnected() const noexcept { return m_owner != nullptr && m_id != 0; }
    void reset() { m_owner = nullptr; m_id = 0; }

private:
    template<typename... Args> friend class Emitter;
    class OwnerBase {
    public:
        virtual ~OwnerBase() = default;
        virtual bool disconnectById(u64 id) = 0;
    };
    OwnerBase* m_owner{nullptr};
    u64        m_id{0};
    Connection(OwnerBase* owner, u64 id) : m_owner(owner), m_id(id) {}
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
        return Connection(&m_owner, id);
    }

    /// Emit: invoke all bound reactions with the given arguments.
    void emit(Args... args) const {
        auto copy = m_connections;
        for (const auto& p : copy) {
            if (p.second) p.second(args...);
        }
    }

    [[nodiscard]] std::size_t reactionCount() const noexcept { return m_connections.size(); }

private:
    struct Owner final : Connection::OwnerBase {
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
    mutable Owner m_owner;
    u64 m_nextId{0};
    std::vector<std::pair<u64, Reaction>> m_connections;

    void fixOwner() const { m_owner.parent = const_cast<Emitter*>(this); }
};

inline void Connection::disconnect() {
    if (m_owner && m_id != 0) {
        m_owner->disconnectById(m_id);
        m_owner = nullptr;
        m_id = 0;
    }
}

} // namespace vapi::ui

#endif // VAPI_UI_EMITTER_HPP

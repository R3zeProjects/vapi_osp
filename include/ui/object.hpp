/** @file object.hpp
 *  @brief UI object tree: parent/child, object name, destroyed notification.
 *  Original implementation. */

#ifndef VAPI_UI_OBJECT_HPP
#define VAPI_UI_OBJECT_HPP

#include "ui/emitter.hpp"
#include "core/types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace vapi::ui {

class Object {
public:
    explicit Object(Object* parent = nullptr);
    virtual ~Object();

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    void setObjectName(std::string name) { m_objectName = std::move(name); }
    [[nodiscard]] const std::string& objectName() const { return m_objectName; }

    void setParent(Object* parent);
    [[nodiscard]] Object* parent() const { return m_parent; }
    [[nodiscard]] Object* parent() { return m_parent; }

    /// Take ownership of a raw child (e.g. from new Child(this)).
    void addChildRaw(Object* child);
    void addChild(std::unique_ptr<Object> child);
    std::unique_ptr<Object> takeChild(Object* child);
    [[nodiscard]] const std::vector<std::unique_ptr<Object>>& children() const { return m_children; }
    [[nodiscard]] std::vector<Object*> childPointers() const;

    /// Emitted when this object is about to be destroyed (before destructor body).
    using DestroyedEmitter = Emitter<>;
    [[nodiscard]] DestroyedEmitter& destroyed() { return m_destroyed; }
    [[nodiscard]] const DestroyedEmitter& destroyed() const { return m_destroyed; }

    [[nodiscard]] Object* findChild(const std::string& name) const;

protected:
    virtual void onParentChanged(Object* oldParent, Object* newParent);
    virtual void onChildAdded(Object* child);
    virtual void onChildRemoved(Object* child);

private:
    std::string m_objectName;
    Object* m_parent{nullptr};
    std::vector<std::unique_ptr<Object>> m_children;
    mutable DestroyedEmitter m_destroyed;
};

} // namespace vapi::ui

#endif // VAPI_UI_OBJECT_HPP

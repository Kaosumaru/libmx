#ifndef MXDRAGSYSTEM
#define MXDRAGSYSTEM
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include <map>
#include <set>

namespace MX
{
namespace Widgets
{

    struct DragLayer
    {
    };

    class DragInfo : public shared_ptr_init<DragInfo>
    {
    };

    class DropTarget;
    class DragTarget;

    class DragSystem : public shared_ptr_init<DragSystem>
    {
    public:
        friend class DropTarget;
        friend class DragTarget;
        DragSystem();
        void Draw();

        static DragSystem& current()
        {
            return ScopeSingleton<DragSystem>::current();
        }

        template <typename T>
        static std::shared_ptr<T> Get()
        {
            auto it = DragSystem::current()._registeredTypes.find(ClassID<T>::id());
            if (it == DragSystem::current()._registeredTypes.end())
                return nullptr;
            return std::static_pointer_cast<T>(it->second);
        }

        template <typename T>
        void RegisterData(const std::shared_ptr<T>& dragInfo)
        {
            _registeredTypes[ClassID<T>::id()] = dragInfo;
        }

        bool dragging() { return _draggedObject != nullptr; }
        Signal<void(const glm::vec2&)> on_started_drag;
        Signal<void(const glm::vec2&)> on_moved_drag;
        Signal<void(const glm::vec2&)> on_ended_drag;

    protected:
        bool StartDragging(DragTarget* object, const glm::vec2& initialPosition, const glm::vec2& offset = {});
        void ChangedDragPosition(DragTarget* object, const glm::vec2& position);
        void StopDragging(DragTarget* object);

        void UnregisterAllTypes();

        std::map<ClassID<>::type, DragInfo::pointer> _registeredTypes;
        DragTarget* _draggedObject;
        glm::vec2 _position;
        glm::vec2 _offset;
    };

    class DragTarget
    {
    public:
        DragTarget() { _dragging = false; }
        virtual ~DragTarget() { InitiateDrop(); }

        bool isDragged() { return _dragging; }

        virtual void DrawDrag(float x, float y) { }

        Signal<void()> onDraggingThis;
        Signal<void()> onDroppedThis;

        template <typename T, typename... Args>
        auto EmplaceDragData(Args&&... args)
        {
            auto data = std::make_shared<T>(std::forward<Args>(args)...);
            onDraggingThis.connect([=]() {
                MX::Widgets::DragSystem::current().RegisterData(data);
            });
            return data;
        }

    protected:
        bool InitiateDrag(const glm::vec2& position, const glm::vec2& offset = {});
        void ChangedDragPosition(const glm::vec2& position);
        void InitiateDrop();

        bool _dragging;
    };

    class DropTarget
    {
    public:
        virtual ~DropTarget() { }

        bool interestedInDrop() { return false; }

        template <typename T>
        inline void RegisterHandler(const std::function<void(void)>& handler)
        {
            RegisterHandler(ClassID<T>::id(), handler);
        }

        template <typename T, typename Func>
        inline void RegisterDropHandler(const Func& func)
        {
            auto handler = [=]() {
                func(MX::Widgets::DragSystem::Get<T>());
            };
            RegisterHandler(ClassID<T>::id(), handler);
        }

        void RegisterHandler(ClassID<>::type type, const std::function<void(void)>& handler) { _dropHandlers.insert(std::make_pair(type, handler)); }

    protected:
        void InitiateGotDrop();
        std::multimap<ClassID<>::type, std::function<void(void)>> _dropHandlers;
    };

}
}

#endif

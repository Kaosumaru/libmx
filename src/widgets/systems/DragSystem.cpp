#include "DragSystem.h"
#include "application/Window.h"
#include "game/resources/Resources.h"
#include <iostream>

using namespace MX;
using namespace MX::Widgets;

bool DragTarget::InitiateDrag(const glm::vec2& position, const glm::vec2& offset)
{
    if (DragSystem::current().StartDragging(this, position, offset))
    {
        _dragging = true;
        onDraggingThis();
        return true;
    }
    return false;
}

void DragTarget::ChangedDragPosition(const glm::vec2& position)
{
    if (_dragging)
        DragSystem::current().ChangedDragPosition(this, position);
}

void DragTarget::InitiateDrop()
{
    if (!_dragging)
        return;
    _dragging = false;
    onDroppedThis();
    DragSystem::current().StopDragging(this);
}

void DropTarget::InitiateGotDrop()
{
    auto& m1 = DragSystem::current()._registeredTypes;
    auto& m2 = _dropHandlers;

    //calls drop handlers for elements in _registeredTypes
    auto it1 = m1.begin();
    auto it2 = m2.begin();
    while (it1 != m1.end() && it2 != m2.end())
    {
        if (it1->first > it2->first)
            it2++;
        else if (it1->first < it2->first)
            it1++;
        else
        {
            do
            {
                it2->second();
                it2++;
                if (it2 == m2.end())
                    return;
            } while (it2->first == it1->first);
            it1++;
        }
    }
}

DragSystem::DragSystem()
{
    _draggedObject = nullptr;
}

void DragSystem::Draw()
{
    if (_draggedObject)
        _draggedObject->DrawDrag(_position.x + _offset.x, _position.y + _offset.y);
}

bool DragSystem::StartDragging(DragTarget* object, const glm::vec2& initialPosition, const glm::vec2& offset)
{
    if (_draggedObject)
        return false;
    _draggedObject = object;
    _position = initialPosition;
    _offset = offset;
    on_started_drag(_position);
    return true;
}

void DragSystem::ChangedDragPosition(DragTarget* object, const glm::vec2& position)
{
    _position = position;
    on_moved_drag(_position);
}

void DragSystem::StopDragging(DragTarget* object)
{
    if (_draggedObject != object)
        return;
    on_ended_drag(_position);
    UnregisterAllTypes();
    _draggedObject = nullptr;
}

void DragSystem::UnregisterAllTypes()
{
    _registeredTypes.clear();
}
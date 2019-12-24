#pragma once

#include "SpriteActor.h"
#include "graphic/images/Image.h"
#include "scene/Scene.h"
#include "utils/FunctorsQueue.h"
#include <list>

namespace MX
{

//basic class
class SpriteScene : public Scene, public virtual SpriteActor
{
public:
    SpriteScene();

    void Run() override;
    void Draw(float x, float y) override;
    void DrawCustom(float x, float y) override;

    std::shared_ptr<Actor> cloneActor() override { return clone(); }

    virtual void AddActor(const SpriteActorPtr& actor);
    virtual void AddActorAtFront(const SpriteActorPtr& actor);

    virtual float Width() { return 0.0f; } //WIP unsigned
    virtual float Height() { return 0.0f; }

    virtual void Clear() {}

    void SetVisible(bool visible);
    bool visible();

    virtual void ChangeVisibility(int i = 1);

    bool position_relative() { return true; }

    virtual void translate_child_position(glm::vec2& position);

    virtual glm::vec2 from_screen_point(const glm::vec2& position) { return position; }
    virtual void for_each_child(const std::function<void(const std::shared_ptr<SpriteActor>&)>& functor) {};

    template <typename T>
    void for_each(const std::function<void(const std::shared_ptr<T>&)>& functor)
    {
        for_each_child([&](const std::shared_ptr<SpriteActor>& actor) {
            auto pointer = std::dynamic_pointer_cast<T>(actor);
            if (pointer)
                functor(pointer);
        });
    }

    template <typename T>
    void for_each_static_cast(const std::function<void(const std::shared_ptr<T>&)>& functor)
    {
        for_each_child([&](const std::shared_ptr<SpriteActor>& actor) {
            auto pointer = std::static_pointer_cast<T>(actor);
            if (pointer)
                functor(pointer);
        });
    }

protected:
    void LinkedActor(const Actor::pointer& actor) override;
    void UnlinkedActor(Actor& actor) override;

    virtual void onBecameVisible();
    virtual void onBecameInvisible();

    bool _visible = true;
    int _visibility = 0; //every sprite scene besides root starts invisible - when it is linked to parent, we decrement it visibility, and we increment it when it is unlinked
};

typedef std::shared_ptr<SpriteScene> SpriteScenePtr;

template <typename T>
class BaseGraphicSceneTemplate : public virtual SpriteScene
{
public:
    using ChildPointer = std::shared_ptr<T>;

    BaseGraphicSceneTemplate()
    {
    }

    void AddActor(const ChildPointer& actor)
    {
        _actors.push_back(actor);
        LinkActor(actor);
    }

    void AddActorAtFront(const ChildPointer& actor)
    {
        _actors.push_front(actor);
        LinkActor(actor);
    }

    void Clear() override
    {
        _actors.clear();
    }

    void Run() override
    {
        if (!visible())
            return;

        auto guard = _actors.guard();
        auto it = _actors.begin();
        while (it != _actors.end())
        {
            auto _safe_copy = (*it); //TODO
            (*it)->Run();
            _actors.temp_advance(it);
        }
    }

    void Draw(float x, float y) override
    {
        if (!visible())
            return;

        auto guard = _actors.guard();
        auto it = _actors.begin();
        while (it != _actors.end())
        {
            (*it)->Draw((*it)->geometry.position.x + x, (*it)->geometry.position.y + y);
            _actors.temp_advance(it);
        }
    }

    void DrawCustom(float x, float y) override
    {
        if (!visible())
            return;

        auto guard = _actors.guard();
        auto it = _actors.begin();
        while (it != _actors.end())
        {
            (*it)->DrawCustom((*it)->geometry.position.x + x, (*it)->geometry.position.y + y);
            _actors.temp_advance(it);
        }
    }

    void for_each_child_type(const std::function<void(const ChildPointer&)>& functor)
    {
        auto guard = _actors.guard();
        auto it = _actors.begin();
        while (it != _actors.end())
        {
            functor(*it);
            _actors.temp_advance(it);
        }
    }

    void for_each_child(const std::function<void(const std::shared_ptr<SpriteActor>&)>& functor) override
    {
        auto guard = _actors.guard();
        auto it = _actors.begin();
        while (it != _actors.end())
        {
            functor(*it);
            _actors.temp_advance(it);
        }
    }

    void CloneChildrenTo(const std::shared_ptr<BaseGraphicSceneTemplate<T>>& scene) const
    {
        CloneChildrenTo(*scene);
    }

    void CloneChildrenTo(BaseGraphicSceneTemplate<T>& scene) const
    {
        //TODO2
        auto self = const_cast<BaseGraphicSceneTemplate<T>*>(this);

        auto guard = self->_actors.guard();
        auto it = self->_actors.begin();
        while (it != self->_actors.end())
        {
            auto& child = *it;
            scene.AddActor(child->clone());
            self->_actors.temp_advance(it);
        }
    }

    bool empty() override { return _actors.empty(); }

protected:
    StrongList<T> _actors;
};

class BaseGraphicScene : public BaseGraphicSceneTemplate<SpriteActor>
{
public:
    BaseGraphicScene();
};

class DisplayScene : public BaseGraphicScene
{
public:
    DisplayScene(const glm::vec2& size);
    void Draw(float x = 0.0f, float y = 0.0f) override;
    float Width() override;
    float Height() override;

    void translate_child_position(glm::vec2& position) override {};

protected:
    glm::vec2 _size;
};

class DisplaySceneTimer : public DisplayScene
{
public:
    DisplaySceneTimer(const glm::vec2& size);
    void Run() override;
    void Run(float timeMultiplier);
    void Draw(float x, float y) override;
    virtual void Pause(bool pause);
    bool paused();
    void PauseResume() { Pause(!paused()); }
    FunctorsQueue& queue();
    Time::SimpleTimer& timer();

protected:
    bool _paused;
    std::shared_ptr<Graphic::TargetSurface> _surface;
    Time::SimpleTimer _timer;
    FunctorsQueue _queue;
};

class FullscreenDisplayScene : public DisplaySceneTimer
{
public:
    FullscreenDisplayScene();
    void Run() override;
};

}
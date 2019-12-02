#include "Actor.h"
#include "Scene.h"
#include <iostream>

using namespace MX;
SceneLink::~SceneLink()
{
    disconnect();
}

void SceneLink::disconnect()
{
}

Actor::Actor()
{
    _scene = nullptr;
}

Actor::~Actor()
{
}

void Actor::OnUnlinkedFromScene()
{
}

void Actor::OnLinkedToScene()
{
}

Scene& Actor::scene()
{
    return *_scene;
}

bool Actor::linked()
{
    return _scene != nullptr;
}

void Actor::Unlink()
{
    if (_link)
    {
        auto link = _link.release();

        if (_scene)
        {
            OnUnlinkedFromScene();
            _scene->UnlinkedActor(*this);
            _scene = nullptr;
        }

        link->unlink();
        delete link;
        return;
    }

    if (_scene)
    {
        OnUnlinkedFromScene();
        _scene = nullptr;
    }
}

Actor::pointer Actor::fromFunctor(const std::function<void(void)>& func)
{
    class ActorFunctor : public Actor
    {
    public:
        ActorFunctor(const std::function<void(void)>& func)
            : _func(func)
        {
        }

        void Run() { _func(); }

        std::shared_ptr<Actor> cloneActor() override { return std::make_shared<ActorFunctor>(_func); }

    protected:
        std::function<void(void)> _func;
    };
    return std::make_shared<ActorFunctor>(func);
}

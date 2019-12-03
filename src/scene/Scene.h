#pragma once
#include "Actor.h"
#include "utils/StrongList.h"
#include <list>

namespace MX
{

class Scene : public virtual Actor
{
    friend class SceneManager;
    friend class Actor;

public:
    virtual ~Scene();

    //virtual void AddActor(const Actor::pointer &actor) = 0; TODO?

protected:
    void LinkActor(const Actor::pointer& actor);

    virtual void LinkedActor(const Actor::pointer& actor) { }
    virtual void UnlinkedActor(Actor& actor) { }
};

class BaseScene : public Scene
{
public:
    BaseScene();
    virtual void AddActor(const Actor::pointer& actor);
    virtual void AddActorAtFront(const Actor::pointer& actor);

    void Run() override;

    const StrongList<Actor>& actors() const;
    bool empty() override { return _actors.empty(); }

protected:
    StrongList<Actor> _actors;
};

struct SceneInfo
{
    SceneInfo()
        : interrupt(false)
    {
    }
    bool interrupt;
};

class InfoScene : public BaseScene
{
public:
    void Run() override;
    bool RunInterrupted();
};

}
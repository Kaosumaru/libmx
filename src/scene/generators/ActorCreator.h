#pragma once
#include "scene/sprites/ScriptableSpriteActor.h"
#include "scene/sprites/SpriteScene.h"
#include "script/PropertyLoaders.h"
#include "script/ScriptObject.h"
#include "utils/Random.h"

namespace MX
{

class ActorCreator : public shared_ptr_init<ActorCreator>, public ScriptObject
{
public:
    ActorCreator()
    {
    }

    ActorCreator(LScriptObject& script)
    {
        script.load_property(_remainingToGenerate, "Max");
        _maxActors = _remainingToGenerate;
    }

    std::shared_ptr<ScriptableSpriteActor> CreateActor()
    {
        if (_maxActors == 0)
            return onCreateActor();
        if (_remainingToGenerate == 0)
            return nullptr;
        _remainingToGenerate--;
        return onCreateActor();
    };

    unsigned remaining() { return _remainingToGenerate; }
    unsigned maxActors() { return _maxActors; }
    bool enabled() { return _maxActors == 0 || _remainingToGenerate > 0; }

    virtual ActorCreator::pointer clone() { return nullptr; }

protected:
    virtual std::shared_ptr<ScriptableSpriteActor> onCreateActor() = 0;

    unsigned _remainingToGenerate = 0;
    unsigned _maxActors = 0;
};

class ActorCreatorEntry
{
public:
    ActorCreatorEntry()
    {
    }
    ActorCreatorEntry(const std::shared_ptr<ScriptableSpriteActor>& obj)
    {
        _actor = obj;
    }

    ActorCreatorEntry(const std::shared_ptr<ActorCreator>& obj)
    {
        _creator = obj;
    }

    std::shared_ptr<ScriptableSpriteActor> CreateActor()
    {
        if (_actor)
            return _actor->cloneSprite();
        if (_creator)
            return _creator->CreateActor();
        return nullptr;
    }

protected:
    std::shared_ptr<ScriptableSpriteActor> _actor;
    std::shared_ptr<ActorCreator> _creator;
};

template <>
struct PropertyLoader<ActorCreatorEntry>
{
    using type = PropertyLoader_Standard;
    static bool load(ActorCreatorEntry& out, const Scriptable::Value::pointer& obj)
    {
        std::shared_ptr<ScriptableSpriteActor> actor;
        if (PropertyLoader<std::shared_ptr<ScriptableSpriteActor>>::load(actor, obj))
        {
            out = ActorCreatorEntry(actor);
            return true;
        }

        std::shared_ptr<ActorCreator> creator;
        if (PropertyLoader<std::shared_ptr<ActorCreator>>::load(creator, obj))
        {
            out = ActorCreatorEntry(creator);
            return true;
        }

        return false;
    }
};

class SpecificActorCreator : public ActorCreator
{
public:
    SpecificActorCreator()
    {
    }

    SpecificActorCreator(LScriptObject& script);

    std::shared_ptr<ScriptableSpriteActor> onCreateActor() override
    {
        if (_actors.empty())
            return nullptr;
        auto& p = _actors.back();
        auto actor = p.first.CreateActor();
        auto& amount = p.second;
        amount--;
        if (amount == 0)
            _actors.pop_back();
        return actor;
    }

    ActorCreator::pointer clone() override { return std::make_shared<SpecificActorCreator>(*this); }

    using Entry = ActorCreatorEntry;
    std::vector<std::pair<Entry, int>> _actors;
};

class RandomActorCreator : public ActorCreator
{
protected:
    using RandomActor = RandomItem<ScriptableSpriteActor>;

public:
    RandomActorCreator();
    RandomActorCreator(LScriptObject& script);

    std::shared_ptr<ScriptableSpriteActor> onCreateActor() override
    {
        return _randomActors.randomItem()->cloneSprite();
    }

    ActorCreator::pointer clone() override { return std::make_shared<RandomActorCreator>(*this); }

    RandomActor _randomActors;
};

class ActorCreatorInit
{
public:
    static void Init();
};

}

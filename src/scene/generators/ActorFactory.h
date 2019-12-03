#pragma once
#include "scene/sprites/ScriptableSpriteActor.h"
#include "scene/sprites/SpriteScene.h"

namespace MX
{

class ActorCreator;
class ActorDecorator;
class ActorGenerator;

class ActorFactory : public ScriptableSpriteActor, public ScopeSingleton<ActorFactory>
{
public:
    ActorFactory() {};
    ActorFactory(const ActorFactory& other);
    ActorFactory(LScriptObject& script);

    void Draw(float x, float y);
    void Run();

    std::shared_ptr<ScriptableSpriteActor> cloneSprite();

    const std::shared_ptr<ActorGenerator>& generator() { return _generator; }
    const std::shared_ptr<ActorCreator>& creator() { return _creator; }
    const std::list<std::shared_ptr<ActorDecorator>>& decorators() { return _decorators; }

    void SetGenerator(const std::shared_ptr<ActorGenerator>& generator) { _generator = generator; }
    void SetCreator(const std::shared_ptr<ActorCreator>& creator) { _creator = creator; }
    void AddDecorator(const std::shared_ptr<ActorDecorator>& decorator) { _decorators.push_back(decorator); }

protected:
    virtual std::shared_ptr<ScriptableSpriteActor> generateActor();

    std::shared_ptr<ActorGenerator> _generator;
    std::shared_ptr<ActorCreator> _creator;
    std::list<std::shared_ptr<ActorDecorator>> _decorators;
};

class ActorGeneratorInit
{
public:
    static void Init();
};

}

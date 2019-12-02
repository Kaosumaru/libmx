#include "ActorFactory.h"
#include "scene/sprites/ScriptableSpriteActor.h"
#include "utils/Random.h"

#include "ActorCreator.h"
#include "ActorDecorator.h"
#include "ActorGenerator.h"
#include "script/ScriptClassParser.h"

using namespace MX;
using namespace MX::Graphic;

ActorFactory::ActorFactory(const ActorFactory& other)
{
    for (auto& decorator : other._decorators)
        _decorators.push_back(decorator->clone());
    _generator = other._generator ? other._generator->clone() : nullptr;
    _creator = other._creator ? other._creator->clone() : nullptr;
}

ActorFactory::ActorFactory(LScriptObject& script)
{
    script.load_property(_decorators, "Decorators");
    script.load_property(_creator, "Creator");
    script.load_property(_generator, "Generator");

    std::shared_ptr<ActorDecorator> decorator;
    if (script.load_property(decorator, "Decorator"))
        _decorators.push_front(decorator);
}

void ActorFactory::Draw(float x, float y)
{
}

std::shared_ptr<ScriptableSpriteActor> ActorFactory::generateActor()
{
    auto actor = _creator->CreateActor();
    if (!actor)
        return nullptr;

    for (auto& decorator : _decorators)
        decorator->DecorateActor(actor);

    return actor;
}

void ActorFactory::Run()
{
    if (_creator && !_creator->enabled())
    {
        Unlink();
        return;
    }
    auto guard = Context<ActorFactory>::Lock(this);
    int particles = _generator->generate();
    for (int i = 0; i < particles; i++)
    {
        auto actor = generateActor();

        if (actor)
            sprite_scene().AddActor(actor);
    }
    ScriptableSpriteActor::Run();
}

std::shared_ptr<ScriptableSpriteActor> ActorFactory::cloneSprite()
{
    return std::make_shared<ActorFactory>(*this);
}

void ActorDecoratorPositionInFactory::DecorateActor(const std::shared_ptr<ScriptableSpriteActor>& actor)
{
    actor->geometry.position = ScopeSingleton<ActorFactory>::current().sprite_scene().geometry.position; //TODO check if this is correct

    if (_offset)
        actor->geometry.position += MX::CreateVectorFromAngle(Random::randomRange((float)(MX_PI * 2))) * _offset;
}

void ActorGeneratorInit::Init()
{
    ScriptClassParser::AddCreator(L"Scene.ActorFactory", new OutsideScriptClassCreatorContructor<ActorFactory>());

    ScriptClassParser::AddCreator(L"Scene.ActorFactory.Generator.Max", new OutsideScriptClassCreatorContructor<ActorGeneratorMax>());
    ScriptClassParser::AddCreator(L"Scene.ActorFactory.Generator.PerSecond", new OutsideScriptClassCreatorContructor<ActorGeneratorSimple>());
    ScriptClassParser::AddCreator(L"Scene.ActorFactory.Generator.RandomIntervals", new OutsideScriptClassCreatorContructor<ActorGeneratorRandomIntervals>());
    ScriptClassParser::AddCreator(L"Scene.ActorFactory.Generator.Incremental", new OutsideScriptClassCreatorContructor<ActorGeneratorIncremental>());

    ScriptClassParser::AddCreator(L"Scene.ActorFactory.Decorator.Scripts", new OutsideScriptClassCreatorContructor<ActorDecoratorScripts>());
    ScriptClassParser::AddCreator(L"Scene.ActorFactory.Decorator.Geometry", new OutsideScriptClassCreatorContructor<ActorDecoratorGeometry>());
    ScriptClassParser::AddCreator(L"Scene.ActorFactory.Decorator.PositionInFactory", new OutsideScriptClassCreatorContructor<ActorDecoratorPositionInFactory>());

    ActorCreatorInit::Init();
}

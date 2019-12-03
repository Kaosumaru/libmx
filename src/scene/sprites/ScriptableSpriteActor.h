#ifndef SCRIPTABLESPRITEACTOR
#define SCRIPTABLESPRITEACTOR
#include "scene/script/Command.h"
#include "scene/script/CommandSignal.h"
#include "scene/script/ScriptableActor.h"
#include "scene/sprites/SpriteActor.h"
#include "scene/sprites/SpriteScene.h"
#include "script/ScriptObject.h"

namespace MX
{
namespace Graphic
{
    class Blender;
}
}

MX::Graphic::Blender& operator<<=(MX::Graphic::Blender& geometry, const MX::Scriptable::Value& value);

namespace MX
{

class ScriptableSpriteActor : public virtual SpriteActor, public ScriptObject, public ScopeSingleton<ScriptableSpriteActor>, public shared_ptr_init<ScriptableSpriteActor>
{
public:
    ScriptableSpriteActor() { }
    ScriptableSpriteActor(const ScriptableSpriteActor& other);
    ScriptableSpriteActor(LScriptObject& script);

    void Run() override;
    void Unlink() override;

    ScriptData script;

    SpriteActorPtr clone() override;
    virtual std::shared_ptr<ScriptableSpriteActor> cloneSprite();

protected:
    void OnLinkedToScene() override;
};

class BaseGraphicSceneScriptable : public BaseGraphicScene, public ScriptableSpriteActor
{
public:
    BaseGraphicSceneScriptable() { }
    BaseGraphicSceneScriptable(const BaseGraphicSceneScriptable& other);
    BaseGraphicSceneScriptable(LScriptObject& script);

    void Run() override;
    void Draw(float x, float y) override;

    std::shared_ptr<Actor> cloneActor() override { return cloneSprite(); }
    SpriteActorPtr clone() override { return cloneSprite(); }
    std::shared_ptr<ScriptableSpriteActor> cloneSprite() override { return std::make_shared<BaseGraphicSceneScriptable>(*this); }

    void Unlink() override;

protected:
    void OnLinkedToScene() override;
};

class BaseGraphicTransformSceneScriptable : public BaseGraphicSceneScriptable
{
public:
    BaseGraphicTransformSceneScriptable(const BaseGraphicTransformSceneScriptable& other)
        : BaseGraphicSceneScriptable(other)
    {
    }
    BaseGraphicTransformSceneScriptable(LScriptObject& script)
        : BaseGraphicSceneScriptable(script)
    {
    }

    void Draw(float x, float y) override;
    void DrawCustom(float x, float y) override;

    std::shared_ptr<ScriptableSpriteActor> cloneSprite() override { return std::make_shared<BaseGraphicTransformSceneScriptable>(*this); }

protected:
};

class ScAnimatedSpriteActor : public AnimatedSpriteAdapter<ScriptableSpriteActor>
{
public:
    ScAnimatedSpriteActor(const ScAnimatedSpriteActor& other);
    ScAnimatedSpriteActor(LScriptObject& script);
    ScAnimatedSpriteActor(const Graphic::Animation::pointer& animation = nullptr);

    std::shared_ptr<ScriptableSpriteActor> cloneSprite() override { return std::make_shared<ScAnimatedSpriteActor>(*this); }
};

class ScSingleRunAnimatedSpriteActor : public SingleRunAnimatedSpriteAdapter<ScriptableSpriteActor>
{
public:
    ScSingleRunAnimatedSpriteActor(const ScSingleRunAnimatedSpriteActor& other);
    ScSingleRunAnimatedSpriteActor(LScriptObject& script);
    ScSingleRunAnimatedSpriteActor(const Graphic::Animation::pointer& animation = nullptr, bool freeze_last_frame = false);

    std::shared_ptr<ScriptableSpriteActor> cloneSprite() override { return std::make_shared<ScSingleRunAnimatedSpriteActor>(*this); }
};

class ScImageSpriteActor : public ImageSpriteAdapter<ScriptableSpriteActor>
{
public:
    ScImageSpriteActor(const ScImageSpriteActor& other);
    ScImageSpriteActor(LScriptObject& script);
    ScImageSpriteActor(const std::shared_ptr<Graphic::Image>& image = nullptr);

    std::shared_ptr<ScriptableSpriteActor> cloneSprite() override { return std::make_shared<ScImageSpriteActor>(*this); }
};

class ScRandomImageSpriteActor : public ImageSpriteAdapter<ScriptableSpriteActor>
{
public:
    ScRandomImageSpriteActor(const ScRandomImageSpriteActor& other);
    ScRandomImageSpriteActor(LScriptObject& script);
    ScRandomImageSpriteActor(const Graphic::SingleAnimation::pointer& animation = nullptr);

    std::shared_ptr<ScriptableSpriteActor> cloneSprite() override { return std::make_shared<ScRandomImageSpriteActor>(*this); }

protected:
    void SetAnimation(const Graphic::SingleAnimation::pointer& animation);

    Graphic::SingleAnimation::pointer _animation;
};

class ScriptableSpriteActorInit
{
public:
    static void Init();
};

}

#endif

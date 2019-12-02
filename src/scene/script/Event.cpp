#include "Event.h"
#include "scene/managers/SceneStackManager.h"
#include "scene/sprites/ScriptableSpriteActor.h"
#include "script/ScriptClassParser.h"
#include "widgets/Widget.h"
#include "widgets/layouters/StackWidget.h"

using namespace MX;

class ReturnEvent : public Event
{
public:
    ReturnEvent(const std::string& objectName)
        : Event(objectName)
    {
        load_property_child(_value, "Value");
    }

    void Do() override
    {
        if (!Context<EventInfo>::isCurrent())
            return;

        auto& info = Context<EventInfo>::current();
        info.returnValue = _value;
    }

protected:
    int _value = 0;
};

class IfEvent : public Event
{
public:
    IfEvent(const std::string& objectName)
        : Event(objectName)
    {
        load_property(_condition, "Condition");
        load_property_children(_thenEvents, "Then");
        load_property(_elseEvents, "Else");
    }

    void Do() override
    {
        float value = *_condition;

        if (value)
            _thenEvents.Do();
        else
            _elseEvents.Do();
    }

protected:
    Scriptable::Value::pointer _condition;
    MX::EventHolder _thenEvents;
    MX::EventHolder _elseEvents;
};

class DelayEvent : public Event
{
public:
    DelayEvent(const std::string& objectName)
        : Event(objectName)
    {
        load_property(_time, "Time");
        load_property_children(_events, "Events");
    }

    void Do() override
    {
        FunctorsQueue::current().planWeakFunctor(
            _time, [&]() { _events.Do(); }, shared_from_this());
    }

protected:
    float _time = 1.0f;
    MX::EventHolder _events;
};
MXREGISTER_CLASS_DEFAULT(L"Event.CompositeDelay", DelayEvent)

class DoEvent : public Event
{
public:
    DoEvent(const std::string& objectName)
        : Event(objectName)
    {
        load_property_child(_do, "Do");
    }

    void Do() override
    {
        float value = *_do;
    }

protected:
    Scriptable::Value::pointer _do;
};

class OnRunEvent : public Event
{
public:
    OnRunEvent(const std::string& objectName)
        : Event(objectName)
    {
        load_property_children(onRun, "Commands");
    }

    void Do() override
    {
        auto& actor = Context<ScriptableSpriteActor>::current();
        actor.script.onRun += onRun;
    }

protected:
    CommandSignal onRun;
};

class StackWidgetPopEvent : public Event
{
public:
    using Event::Event;

    void Do() override
    {
        auto& widget = Context<MX::Widgets::Widget>::current();
        auto stack = MX::Widgets::StackWidget::stack_of(&widget);
        if (stack)
            stack->PopWidget();
    }
};

class SpriteSceneStackManagerPopEvent : public Event
{
public:
    using Event::Event;

    void Do() override
    {
        auto& sprite = ScriptableSpriteActor::current();
        SpriteSceneStackManager::PopOnManagerOf(&sprite);
    }
};

class CreateSpriteAtSprite : public Event
{
public:
    CreateSpriteAtSprite(const std::string& objectName)
        : Event(objectName)
    {
        load_property_child(_actor, "Sprite");
        load_property(_rotateToCurrent, "RotateToCurrent");
    }

    void Do()
    {
        if (!ScriptableSpriteActor::isCurrent())
            return;

        if (!_actor)
            return;
        auto actor = _actor->cloneSprite();

        actor->geometry.position = ScriptableSpriteActor::current().geometry.position;

        if (_rotateToCurrent)
            actor->geometry.angle = ScriptableSpriteActor::current().geometry.angle;

        ScriptableSpriteActor::current().sprite_scene().AddActor(actor);
    }

protected:
    std::shared_ptr<MX::ScriptableSpriteActor> _actor;
    bool _rotateToCurrent = false;
};

class SoundEffectEvent : public Event
{
public:
    SoundEffectEvent(const std::string& objectName)
        : Event(objectName)
    {
        load_property_child(_sound, "Sound");
    }

    void Do()
    {
        if (_sound)
            _sound->sound()->Play();
    }

protected:
    std::shared_ptr<MX::ScriptSoundClass> _sound;
};

void EventInit::Init()
{
    ScriptClassParser::AddCreator(L"Event.Return", new DefaultClassCreatorContructor<ReturnEvent>());
    ScriptClassParser::AddCreator(L"Event.If", new DefaultClassCreatorContructor<IfEvent>());
    ScriptClassParser::AddCreator(L"Event.Do", new DefaultClassCreatorContructor<DoEvent>());
    ScriptClassParser::AddCreator(L"Event.OnRun", new DefaultClassCreatorContructor<OnRunEvent>());

    ScriptClassParser::AddCreator(L"Event.StackWidget.Pop", new DefaultClassCreatorContructor<StackWidgetPopEvent>());

    ScriptClassParser::AddCreator(L"Event.Scene.StackManager.Pop", new DefaultClassCreatorContructor<SpriteSceneStackManagerPopEvent>());
    ScriptClassParser::AddCreator(L"Event.Sprite.CreateSprite", new DefaultClassCreatorContructor<CreateSpriteAtSprite>());

    ScriptClassParser::AddCreator(L"Event.PlaySound", new DefaultClassCreatorContructor<SoundEffectEvent>());
}

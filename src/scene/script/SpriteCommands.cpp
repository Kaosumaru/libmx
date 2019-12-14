#include "SpriteCommands.h"
#include "CommonCommands.h"
#include "Event.h"
#include "script/ScriptClassParser.h"
#include "script/class/ScriptSoundClass.h"
#include "utils/Random.h"
#include "utils/Time.h"

namespace MX
{

using namespace std;

class UnlinkCommand : public Command
{
public:
    static Command::pointer get_static()
    {
        static Command::pointer ptr = std::make_shared<UnlinkCommand>();
        return ptr;
    }

    bool operator()() override
    {
        ScriptableSpriteActor::current().StopThenUnlink();
        return false;
    }

    Command::pointer clone() override { return get_static(); }
};

class UnlinkIfEmptyCommand : public Command
{
public:
    static Command::pointer get_static()
    {
        static Command::pointer ptr = std::make_shared<UnlinkIfEmptyCommand>();
        return ptr;
    }

    bool operator()() override
    {
        if (ScriptableSpriteActor::current().empty())
        {
            ScriptableSpriteActor::current().Unlink();
            return false;
        }
        return true;
    }

    Command::pointer clone() override { return get_static(); }
};

std::shared_ptr<Command> unlink()
{
    return UnlinkCommand::get_static();
}

std::shared_ptr<Command> unlink_if_empty()
{
    return UnlinkIfEmptyCommand::get_static();
}

class ChangeColorCommand : public Command
{
public:
    ChangeColorCommand(const Color& color)
        : _color(color)
    {
    }
    ChangeColorCommand(LScriptObject& script)
    {
        script.load_property(_color, "Color");
    }

    bool operator()() override
    {
        ScriptableSpriteActor::current().geometry.color = _color;
        return false;
    }

    Command::pointer clone() override { return std::make_shared<ChangeColorCommand>(_color); }

protected:
    Color _color;
};

std::shared_ptr<Command> change_color(const Color& color)
{
    return make_shared<ChangeColorCommand>(color);
}

class TeleportToCommand : public Command
{
public:
    TeleportToCommand(float x, float y)
        : _vec(x, y)
    {
    }
    TeleportToCommand(const glm::vec2& vec)
        : _vec(vec)
    {
    }
    TeleportToCommand(LScriptObject& script)
    {
        script.load_property(_vec, "Position");
    }

    bool operator()() override
    {
        ScriptableSpriteActor::current().geometry.position = _vec;
        return false;
    }

    Command::pointer clone() override { return std::make_shared<TeleportToCommand>(_vec); }

protected:
    glm::vec2 _vec;
};

std::shared_ptr<Command> teleport_to(const glm::vec2& vec)
{
    return make_shared<TeleportToCommand>(vec);
}

std::shared_ptr<Command> teleport_to(float x, float y)
{
    return make_shared<TeleportToCommand>(x, y);
}

class LerpColorCommand : public WaitCommand
{
public:
    LerpColorCommand(const Color& color1, const Color& color2, float seconds)
        : _color1(color1)
        , _color2(color2)
        , WaitCommand(seconds)
    {
    }
    LerpColorCommand(const LerpColorCommand& other)
        : _color1(other._color1)
        , _color2(other._color2)
        , WaitCommand(other)
    {
    }
    LerpColorCommand(LScriptObject& script)
        : WaitCommand(script)
    {
        script.load_property(_color1, "Color1");
        script.load_property(_color2, "Color2");
    }

    bool operator()() override
    {
        bool ret = WaitCommand::operator()();
        ScriptableSpriteActor::current().geometry.color = Color::lerp(_color1, _color2, percent());
        return ret;
    }

    Command::pointer clone() override { return std::make_shared<LerpColorCommand>(*this); }

protected:
    Color _color1;
    Color _color2;
};

class LerpColorFromBaseCommand : public WaitCommand
{
public:
    LerpColorFromBaseCommand(float seconds)
        : WaitCommand(seconds)
    {
    }
    LerpColorFromBaseCommand(LScriptObject& script)
        : WaitCommand(script)
    {
    }

    bool operator()() override
    {
        auto& current = ScriptableSpriteActor::current();

        if (_first_time)
        {
            _first_time = false;
            _originalColor = current.geometry.color;
        }

        bool ret = WaitCommand::operator()();
        ModifyColor(current, _originalColor);
        return ret;
    }

protected:
    virtual void ModifyColor(ScriptableSpriteActor& actor, Color& original)
    {
    }

private:
    bool _first_time = true;
    Color _originalColor;
};

class LerpColor1Command : public LerpColorFromBaseCommand
{
public:
    LerpColor1Command(const Color& color, float seconds)
        : _color(color)
        , LerpColorFromBaseCommand(seconds)
    {
    }
    LerpColor1Command(LScriptObject& script)
        : LerpColorFromBaseCommand(script)
    {
        script.load_property(_color, "Color");
    }

    void ModifyColor(ScriptableSpriteActor& actor, Color& original) override
    {
        ScriptableSpriteActor::current().geometry.color = (Color::lerp(original, _color, percent()));
    }

    Command::pointer clone() override { return std::make_shared<LerpColor1Command>(_color, (float)_seconds); }

protected:
    Color _color;
};

class LerpAlphaCommand : public LerpColorFromBaseCommand
{
public:
    LerpAlphaCommand(const LerpAlphaCommand& other)
        : LerpColorFromBaseCommand(other)
    {
        _alpha = other._alpha;
        _from = other._from;
    }

    LerpAlphaCommand(LScriptObject& script)
        : LerpColorFromBaseCommand(script)
    {
        script.load_property(_alpha, "Alpha");
        script.load_property(_from, "From");
    }

    void ModifyColor(ScriptableSpriteActor& actor, Color& original) override
    {
        auto new_color = original;
        float from = _from == -1 ? original.a() : _from;
        new_color.SetA(Easing::linear(from, _alpha - from, percent()));
        actor.geometry.color = new_color;
    }

    Command::pointer clone() override { return std::make_shared<LerpAlphaCommand>(*this); }

protected:
    float _alpha;
    float _from = -1;
};

std::shared_ptr<Command> lerp_color(const Color& color1, const Color& color2, float seconds)
{
    return make_shared<LerpColorCommand>(color1, color2, seconds);
}

std::shared_ptr<Command> lerp_color(const Color& color, float seconds)
{
    return make_shared<LerpColor1Command>(color, seconds);
}

class BlinkColorCommand : public LerpColorFromBaseCommand
{
public:
    BlinkColorCommand(const Color& color, float seconds)
        : _color(color)
        , LerpColorFromBaseCommand(seconds)
    {
    }
    BlinkColorCommand(LScriptObject& script)
        : LerpColorFromBaseCommand(script)
    {
        script.load_property(_color, "Color");
    }

    void ModifyColor(ScriptableSpriteActor& actor, Color& original) override
    {
        float p = 0.0f;

        if (percent() == 1.0f)
            p = 0.0f;
        else if (percent() > 0.5f)
            p = 1.0f - percent();
        else
            p = percent();

        ScriptableSpriteActor::current().geometry.color = Color::lerp(original, _color * original, exp(p * 2.0f));
    }

    Command::pointer clone() override { return std::make_shared<LerpColor1Command>(_color, (float)_seconds); }

protected:
    Color _color;
};

std::shared_ptr<Command> blink_color(const Color& color, float seconds)
{
    return make_shared<BlinkColorCommand>(color, seconds);
}

class MoveToCommand : public WaitCommand
{
public:
    MoveToCommand(const glm::vec2& vec, float seconds)
        : _vec(vec)
        , WaitCommand(seconds)
    {
    }
    MoveToCommand(LScriptObject& script)
        : WaitCommand(script)
    {
        script.load_property(_vec, "Position");
    }

    void Restart()
    {
        _start = true;
        WaitCommand::Restart();
    }

    bool operator()() override
    {
        if (_start)
        {
            _vecOrig = ScriptableSpriteActor::current().geometry.position;
            _start = false;
        }
        bool ret = WaitCommand::operator()();
        ScriptableSpriteActor::current().geometry.position = MX::lerp(_vecOrig, _vec, percent());
        return ret;
    }

    Command::pointer clone() override { return std::make_shared<MoveToCommand>(_vec, (float)_seconds); }

protected:
    bool _start = true;
    glm::vec2 _vecOrig;
    glm::vec2 _vec;
};

std::shared_ptr<Command> move_to(const glm::vec2& vec, float seconds)
{
    return make_shared<MoveToCommand>(vec, seconds);
}

class WarpScaleCommand : public WaitCommand
{
public:
    WarpScaleCommand(float x, float y, float seconds)
        : _vec(x, y)
        , WaitCommand(seconds)
    {
    }
    WarpScaleCommand(const glm::vec2& vec, float seconds)
        : _vec(vec)
        , WaitCommand(seconds)
    {
    }
    WarpScaleCommand(LScriptObject& script)
        : WaitCommand(script)
    {
        script.load_property(_vec, "Scale");
    }

    void Restart()
    {
        _start = true;
        WaitCommand::Restart();
    }

    bool operator()() override
    {
        if (_start)
        {
            _vecOrig = ScriptableSpriteActor::current().geometry.scale;
            _start = false;
        }
        bool ret = WaitCommand::operator()();
        ScriptableSpriteActor::current().geometry.scale = MX::lerp(_vecOrig, _vec, percent());
        return ret;
    }

    Command::pointer clone() override { return std::make_shared<WarpScaleCommand>(_vec.x, _vec.y, (float)_seconds); }

protected:
    bool _start = true;
    glm::vec2 _vecOrig;
    glm::vec2 _vec;
};

class WarpRelativeScaleCommand : public WaitCommand
{
public:
    WarpRelativeScaleCommand(float x, float y, float seconds)
        : _vec(x, y)
        , WaitCommand(seconds)
    {
    }
    WarpRelativeScaleCommand(const glm::vec2& vec, float seconds)
        : _vec(vec)
        , WaitCommand(seconds)
    {
    }
    WarpRelativeScaleCommand(LScriptObject& script)
        : WaitCommand(script)
    {
        script.load_property(_vec, "Scale");
    }

    void Restart()
    {
        _start = true;
        WaitCommand::Restart();
    }

    bool operator()() override
    {
        if (_start)
        {
            _vecOrig = ScriptableSpriteActor::current().geometry.scale;
            _vec.x *= _vecOrig.x;
            _vec.y *= _vecOrig.y;
            _start = false;
        }
        bool ret = WaitCommand::operator()();
        ScriptableSpriteActor::current().geometry.scale = MX::lerp(_vecOrig, _vec, percent());
        return ret;
    }

    Command::pointer clone() override { return std::make_shared<WarpRelativeScaleCommand>(_vec.x, _vec.y, (float)_seconds); }

protected:
    bool _start = true;
    glm::vec2 _vecOrig;
    glm::vec2 _vec;
};

std::shared_ptr<Command> warp_scale(float x, float y, float seconds)
{
    return make_shared<WarpScaleCommand>(x, y, seconds);
}

class MoveInDirectionCommand : public Command
{
public:
    MoveInDirectionCommand(const glm::vec2& vec, float speed)
    {
        _vec = vec;
        _speed = speed;
    }
    MoveInDirectionCommand(LScriptObject& script)
    {
        script.load_property(_vec, "Direction");
        script.load_property(_speed, "Speed");
    }

    bool operator()() override
    {
        ScriptableSpriteActor::current().geometry.position += _vec * _speed;
        return true;
    }

    Command::pointer clone() override { return std::make_shared<MoveInDirectionCommand>(_vec, _speed.getOriginalValue()); }

protected:
    glm::vec2 _vec;
    Time::FloatPerSecond _speed;
};

class RotateCommand : public Command
{
public:
    RotateCommand(float speed)
    {
        _speed = speed;
    }
    RotateCommand(LScriptObject& script)
    {
        script.load_property_child(_speed, "Speed");
    }

    bool operator()() override
    {
        ScriptableSpriteActor::current().geometry.angle += _speed;
        return true;
    }

    Command::pointer clone() override { return std::make_shared<RotateCommand>(_speed.getOriginalValue()); }

protected:
    Time::FloatPerSecond _speed;
};

std::shared_ptr<Command> rotate(float speed)
{
    return std::make_shared<RotateCommand>(speed);
}

class SetRandomAngle : public Command
{
public:
    SetRandomAngle(float x, float y)
        : _angle(x, y)
    {
    }
    SetRandomAngle(const std::pair<float, float>& angle)
        : _angle(angle)
    {
    }
    SetRandomAngle(LScriptObject& script)
    {
        script.load_property(_angle, "Angle");
    }

    bool operator()() override
    {
        ScriptableSpriteActor::current().geometry.angle = Random::randomRange(_angle) * MX_2PI;
        return false;
    }

    Command::pointer clone() override { return std::make_shared<SetRandomAngle>(_angle); }

protected:
    std::pair<float, float> _angle;
};

class SoundEffect : public Command
{
public:
    SoundEffect(std::shared_ptr<MX::ScriptSoundClass> sound_effect)
        : _sound(sound_effect)
    {
    }
    SoundEffect(LScriptObject& script)
    {
        script.load_property_child(_sound, "Sound");
    }

    bool operator()() override
    {
        if (_sound)
            _sound->sound()->Play();
        return false;
    }

    Command::pointer clone() override { return std::make_shared<SoundEffect>(_sound); }

protected:
    std::shared_ptr<MX::ScriptSoundClass> _sound;
};

std::shared_ptr<Command> move_in_direction(const glm::vec2& vec, float speed)
{
    return make_shared<MoveInDirectionCommand>(vec, speed);
}

class GlueToCommand : public Command
{
public:
    GlueToCommand(const GlueToCommand& other)
    {
        _ptr = other._ptr;
        _options = other._options;
    }

    GlueToCommand(const std::weak_ptr<SpriteActor>& ptr, const glue_to_options& options)
        : _ptr(ptr)
        , _options(options)
    {
    }

    bool operator()() override
    {
        auto ptr = _ptr.lock();
        if (!ptr || !ptr->linked())
        {
            if (!_options.surviveParent)
                ScriptableSpriteActor::current().StopThenUnlink();
            return false;
        }

        if (!_options.absolute_position)
            ScriptableSpriteActor::current().geometry.position = ptr->geometry.position;
        else
            ScriptableSpriteActor::current().geometry.position = ptr->absolute_position();

        if (_options.inherit_scale)
            ScriptableSpriteActor::current().geometry.scale = ptr->geometry.scale;

        if (_options.shouldRotate)
            ScriptableSpriteActor::current().geometry.angle = ptr->geometry.angle;
        return true;
    }

    Command::pointer clone() override { return std::make_shared<GlueToCommand>(*this); }

protected:
    std::weak_ptr<SpriteActor> _ptr;
    glue_to_options _options;
};

class GlueToCommandWithOffset : public GlueToCommand
{
public:
    GlueToCommandWithOffset(const GlueToCommandWithOffset& other)
        : GlueToCommand(other)
    {
    }

    GlueToCommandWithOffset(const std::weak_ptr<SpriteActor>& ptr, const glue_to_options& options)
        : GlueToCommand(ptr, options)
    {
    }

    bool operator()() override
    {
        if (GlueToCommand::operator()())
        {
            auto offset = MX::vectorByRotation(_options.offset, ScriptableSpriteActor::current().geometry.angle);

            auto ptr = _ptr.lock();
            if (_options.scale_offset && ptr)
            {
                offset.x *= ptr->geometry.scale.x;
                offset.y *= ptr->geometry.scale.y;
            }

            ScriptableSpriteActor::current().geometry.position += offset;
            return true;
        }
        return false;
    }

    Command::pointer clone() override { return std::make_shared<GlueToCommandWithOffset>(*this); }

protected:
};

std::shared_ptr<Command> glue_to(const SpriteActorPtr& ptr, const glue_to_options& options)
{
    std::shared_ptr<GlueToCommand> command;
    if (options.offset == glm::vec2 {})
        command = std::make_shared<GlueToCommand>(ptr, options);
    else
        command = std::make_shared<GlueToCommandWithOffset>(ptr, options);
    return command;
}

class NailToCommand : public Command
{
public:
    NailToCommand(const SpriteActorPtr& original, const SpriteActorPtr& ptr)
        : _ptr(ptr)
    {
        auto deltaVector = original->geometry.position - ptr->geometry.position;
        _hitAngle = angle(deltaVector) - ptr->geometry.angle;
        _objectAngle = original->geometry.angle - ptr->geometry.angle;
        _length = length(deltaVector);
    }

    bool operator()() override
    {
        auto ptr = _ptr.lock();
        if (!ptr || !ptr->linked())
        {
            ScriptableSpriteActor::current().StopThenUnlink();
            return false;
        }

        auto angle = ptr->geometry.angle;
        auto deltaVector = CreateVectorFromAngle(_hitAngle + angle) * _length;

        ScriptableSpriteActor::current().geometry.position = ptr->geometry.position + deltaVector;
        ScriptableSpriteActor::current().geometry.angle = _objectAngle + angle;
        return true;
    }

    Command::pointer clone() override { return nullptr; }

protected:
    std::weak_ptr<SpriteActor> _ptr;
    float _hitAngle;
    float _objectAngle;
    float _length;
};

std::shared_ptr<Command> nail_to(const SpriteActorPtr& original, const SpriteActorPtr& ptr)
{
    return std::make_shared<NailToCommand>(original, ptr);
}

class ExecuteEventCommand : public Command
{
public:
    ExecuteEventCommand(const EventPtr& event)
        : _event(event)
    {
    }
    ExecuteEventCommand(LScriptObject& script)
    {
        script.load_property_child(_event, "Event");
    }

    bool operator()() override
    {
        if (_event)
            _event->Do();
        return false;
    }

    Command::pointer clone() override { return std::make_shared<ExecuteEventCommand>(_event); }

protected:
    EventPtr _event;
};

void ScriptableCommandsInit::Init()
{
    ScriptClassParser::AddCreator(L"Command.Unlink", [=](const std::wstring& className, const std::string& instancePath) { return UnlinkCommand::get_static(); });
    ScriptClassParser::AddCreator(L"Command.UnlinkIfEmpty", [=](const std::wstring& className, const std::string& instancePath) { return UnlinkIfEmptyCommand::get_static(); });

    ScriptClassParser::AddCreator(L"Command.ChangeColor", new OutsideScriptClassCreatorContructor<ChangeColorCommand>());
    ScriptClassParser::AddCreator(L"Command.Teleport", new OutsideScriptClassCreatorContructor<TeleportToCommand>());
    ScriptClassParser::AddCreator(L"Command.BlinkColor", new OutsideScriptClassCreatorContructor<BlinkColorCommand>());

    ScriptClassParser::AddCreator(L"Command.LerpAlpha", new OutsideScriptClassCreatorContructor<LerpAlphaCommand>());
    ScriptClassParser::AddCreator(L"Command.LerpColor", new OutsideScriptClassCreatorContructor<LerpColorCommand>());
    ScriptClassParser::AddCreator(L"Command.LerpToColor", new OutsideScriptClassCreatorContructor<LerpColor1Command>());
    ScriptClassParser::AddCreator(L"Command.MoveTo", new OutsideScriptClassCreatorContructor<MoveToCommand>());
    ScriptClassParser::AddCreator(L"Command.WarpScale", new OutsideScriptClassCreatorContructor<WarpScaleCommand>());
    ScriptClassParser::AddCreator(L"Command.WarpRelativeScale", new OutsideScriptClassCreatorContructor<WarpRelativeScaleCommand>());
    ScriptClassParser::AddCreator(L"Command.Move", new OutsideScriptClassCreatorContructor<MoveInDirectionCommand>());
    ScriptClassParser::AddCreator(L"Command.Rotate", new OutsideScriptClassCreatorContructor<RotateCommand>());
    ScriptClassParser::AddCreator(L"Command.SetRandomAngle", new OutsideScriptClassCreatorContructor<SetRandomAngle>());
    ScriptClassParser::AddCreator(L"Command.ExecuteEvent", new OutsideScriptClassCreatorContructor<ExecuteEventCommand>());
    ScriptClassParser::AddCreator(L"Command.PlaySound", new OutsideScriptClassCreatorContructor<SoundEffect>());
}

}
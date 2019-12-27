#pragma once
#include "Command.h"
#include "utils/Time.h"
#include "utils/Utils.h"
#include "utils/graphic/Easings.h"
#include <memory>

namespace MX
{

/*
	** Waits given time, then dies
	*/
class WaitCommand : public Command
{
public:
    WaitCommand(float seconds);
    WaitCommand(LScriptObject& script);
    WaitCommand(const WaitCommand& other);
    void Restart() override;
    bool operator()() override;

    float percent();
    Command::pointer clone() override { return std::make_shared<WaitCommand>(*this); }

protected:
    virtual void OnWait() { }

    float _seconds;
    Time::ManualStopWatch _stopWatch;
};

class EasingWaitCommand : public WaitCommand
{
public:
    EasingWaitCommand(float time)
        : WaitCommand(time)
    {
    }

    EasingWaitCommand(const EasingWaitCommand& other)
        : _easingFunc(other._easingFunc)
        , WaitCommand(other)
    {
    }

    float percent()
    {
        return _easingFunc(0.0f, 1.0f, WaitCommand::percent());
    }

protected:
    Easing::Function _easingFunc = Easing::linear<float>;
};

Command::pointer command_from_functor(const std::function<bool(void)>& f);

class CompositeCommand : public Command
{
public:
    CompositeCommand(const CompositeCommand& other)
    {
        _commands = other._commands;
        for (auto& command : _commands)
            command = command->clone();
    }

    CompositeCommand(LScriptObject& script);

    CompositeCommand(std::list<Command::pointer>&& c)
        : _commands(c)
    {
    }

    void AddCommand(const Command::pointer& c)
    {
        _commands.push_back(c);
    }

    void AddLambda(const std::function<bool(void)>& f)
    {
        AddCommand(command_from_functor(f));
    }

    bool empty() const { return _commands.empty(); }

protected:
    std::list<Command::pointer> _commands;
};

std::shared_ptr<Command> wait(float seconds);

class CommonCommandsInit
{
public:
    static void Init();
};

//std::shared_ptr<Command> l(p0, p1, ..., pn);
std::shared_ptr<CompositeCommand> l(std::list<Command::pointer>&& l);
std::shared_ptr<CompositeCommand> l(int i, std::list<Command::pointer>&& l);
std::shared_ptr<CompositeCommand> s(std::list<Command::pointer>&& l);
std::shared_ptr<CompositeCommand> q(std::list<Command::pointer>&& l);

}

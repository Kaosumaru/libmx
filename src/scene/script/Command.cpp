#include "Command.h"

namespace MX
{

float CommandContext::elapsedTime = 0.0f;

void ScriptHolder::connect_command(const Command::pointer& com, int p)
{
    commands.insert(std::make_pair(p, com));
}

void ScriptHolder::Clear()
{
    commands.clear();
}

void ScriptHolder::operator()()
{
    CommandContext::Reset();
    auto it = commands.begin();
    while (it != commands.end())
    {
        if (!(*(it->second))())
            it = commands.erase(it);
        else
            it++;
    }
}

ScriptHolder& ScriptHolder::operator=(const ScriptHolder& other)
{
    commands = other.commands;
    for (auto& command : commands)
        command.second = const_cast<Command*>(command.second.get())->clone();
    return *this;
}

ScriptHolder& ScriptHolder::operator=(const MX::Scriptable::Value& value)
{
    Clear();
    return operator+=(value);
}

ScriptHolder& ScriptHolder::operator+=(const MX::Scriptable::Value& value)
{
    auto& array = value.array();

    for (auto& value : array)
    {
        auto command = value->to_object<Command>();
        if (command)
            connect_command(command);
    }
    return *this;
}

ScriptHolder& ScriptHolder::operator+=(const ScriptHolder& value)
{
    for (auto& value : value.commands)
    {
        auto command = value.second->clone();
        commands.insert(std::make_pair(value.first, command));
    }
    return *this;
}

void ScriptSignal::operator()()
{
    ScriptHolder::operator()();
    Signal<void(void)>::operator()();
}

ScriptSignal& ScriptSignal::operator=(const ScriptSignal& value)
{
    ScriptHolder::operator=(value);
    return *this;
}
ScriptSignal& ScriptSignal::operator=(const MX::Scriptable::Value& value)
{
    ScriptHolder::operator=(value);
    return *this;
}
ScriptSignal& ScriptSignal::operator+=(const MX::Scriptable::Value& value)
{
    ScriptHolder::operator+=(value);
    return *this;
}
ScriptSignal& ScriptSignal::operator+=(const ScriptHolder& value)
{
    ScriptHolder::operator+=(value);
    return *this;
}

void ScriptSignal::Clear()
{
    disconnect_all_slots();
    ScriptHolder::Clear();
}

}
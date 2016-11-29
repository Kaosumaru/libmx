#pragma once
#include "Command.h"

namespace MX
{


class CommandSignal : public ScriptHolder, public SimpleSignal<>
{
public:
    CommandSignal(){};
    
    
    ScriptHolder& operator = (const MX::Scriptable::Value &value);
    ScriptHolder& operator += (const MX::Scriptable::Value &value);
    ScriptHolder& operator += (const ScriptHolder &value);
    
    void operator () ();
    
    void Clear();
};


}

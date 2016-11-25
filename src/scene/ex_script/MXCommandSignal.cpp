#include "MXCommandSignal.h"
#include "Utils/MXInclude.h"

namespace MX
{

    void CommandSignal::operator () ()
    {
        ScriptHolder::operator()();
        SimpleSignal<>::operator()();
    }
    
    ScriptHolder& CommandSignal::operator = (const MX::Scriptable::Value &value) { return ScriptHolder::operator=(value); }
    ScriptHolder& CommandSignal::operator += (const MX::Scriptable::Value &value) { return ScriptHolder::operator+=(value); }
    ScriptHolder& CommandSignal::operator += (const ScriptHolder &value) { return ScriptHolder::operator+=(value); }
    
    
    void CommandSignal::Clear()
    {
        _functors.clear();
        ScriptHolder::Clear();
    }





}
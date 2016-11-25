#ifndef MXCOMMANDSIGNAL
#define MXCOMMANDSIGNAL
#include "MXCommand.h"
#include "Utils/MXBoostFast.h"

namespace bs2 = boost::signals2;

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


#endif

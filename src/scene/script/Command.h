#pragma once
#include "utils/Utils.h"
#include <memory>
#include <map>
#include "utils/Signal.h"
#include "script/Script.h"
#include "script/ScriptObject.h"


namespace MX
{




class Command : public ScriptObjectString, public shared_ptr_init<Command>
{
public:
    Command() {}
    Command(const std::string& objectName) : ScriptObjectString(objectName) {}
    
	virtual ~Command(){}
	virtual void Restart(){}
    
    //true - still working
    //false - delete this and pass to next
	virtual bool operator()() { return false; }

	virtual Command::pointer clone() { return std::make_shared<Command>(); }
};


class ScriptHolder
{
public:
	ScriptHolder(){};
    virtual ~ScriptHolder(){};

	void connect_command(const Command::pointer& com, int p = 0);
	void operator () ();

	ScriptHolder& operator = (const ScriptHolder &other);
	ScriptHolder& operator = (const MX::Scriptable::Value &value);
	ScriptHolder& operator += (const MX::Scriptable::Value &value);
	ScriptHolder& operator += (const ScriptHolder &value);
    
    virtual void Clear();
protected:
	std::multimap<int, Command::pointer> commands;
};
    
class ScriptSignal : public ScriptHolder, public Signal<void (void)>
{
public:
    ScriptSignal(){};
    
	ScriptSignal& operator = (const ScriptSignal &other);
	ScriptSignal& operator = (const MX::Scriptable::Value &value);
	ScriptSignal& operator += (const MX::Scriptable::Value &value);
	ScriptSignal& operator += (const ScriptHolder &value);
    
    void operator () ();
    
    void Clear();
};


}


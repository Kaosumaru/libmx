#pragma once
#include "Scene/Actor.h"
#include "Command.h"
#include "CommandSignal.h"

namespace MX
{


class ScriptData
{
public:
	ScriptData()
	{

	}

	ScriptData(const ScriptData& other)
	{
		onRun = other.onRun;
		onUnlink = other.onUnlink;
		onLink = other.onLink;
	}

	ScriptData(LScriptObject& script)
	{
		script.load_property(onRun, "On.Run");
		script.load_property(onLink, "On.Link");
		script.load_property(onUnlink, "On.Unlink");
	}


	CommandSignal onRun;
	ScriptSignal onLink;
	ScriptSignal onUnlink;
};

class ScriptableActor : public virtual Actor, public ScopeSingleton<ScriptableActor>
{
public:
	void Run();
	void Unlink();

	CommandSignal onRun;
	ScriptSignal onLink;
	ScriptSignal onUnlink;
protected:
	void OnLinkedToScene();
};


}

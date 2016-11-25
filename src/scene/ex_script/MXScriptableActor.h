#ifndef MXSCRIPTABLEACTOR
#define MXSCRIPTABLEACTOR
#include "Scene/MXActor.h"
#include "MXCommand.h"
#include "MXCommandSignal.h"
#include "Utils/MXBoostFast.h"
#include <boost/signals2/signal.hpp>
namespace bs2 = boost::signals2;

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


#endif

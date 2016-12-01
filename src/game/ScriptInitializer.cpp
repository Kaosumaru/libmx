


#include "ScriptInitializer.h"

#include "Game/Resources/Resources.h"
#include "Widgets/Drawers/Drawers.h"
#ifdef WIP
#include "Widgets/Drawers/ShaderDrawers.h"
#include "Graphic/Effects/EffectActors.h"
#endif
#include "Widgets/Animations/Animations.h"
#include "Widgets/Drawers/ProxyDrawers.h"
#include "Widgets/Drawers/CompositeDrawers.h"
#include "Widgets/Drawers/DestinationDrawers.h"
#include "Widgets/Layouters/ScriptLayouters.h"
#include "Graphic/Particles/Particles.h"
#include "Scene/Script/Event.h"
#include "Script/ScriptClassParser.h"




using namespace MX;

using namespace std;

void MX::ScriptInitializer::Init()
{
	

	MX::ScriptableSpriteActorInit::Init();
	MX::EventInit::Init();

#ifdef WIP
	MX::Widgets::ShaderDrawersInit::Init();
	MX::Graphic::EffectActorsInit::Init();
#endif

	MX::Graphic::ParticleInit::Init();
	MX::Widgets::DrawersInit::Init();
	MX::Widgets::ProxyDrawersInit::Init();
	MX::Widgets::CompositeDrawersInit::Init();
	MX::Widgets::DestinationDrawersInit::Init();
	MX::Widgets::ShapePolicyInit::Init();
	MX::Widgets::ScriptLayouterInit::Init();
	MX::Widgets::WidgetAnimationsInit::Init();


}


void MX::ScriptInitializer::AfterScriptParse()
{

}

void MX::ScriptInitializer::ReloadScripts(bool reset)
{

#ifndef MX_GAME_RELEASE
    static  std::list<std::string> p = { "script/game/", "script/resources/", "script/test/" };
#else
    static  std::list<std::string> p = { "script/game/", "script/resources/", "script/test/" };
#endif

    Script::ParseDirs(p, reset);
    AfterScriptParse();
}
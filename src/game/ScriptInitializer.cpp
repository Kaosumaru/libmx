


#include "ScriptInitializer.h"

#include "Game/Resources/Resources.h"
#include "widgets/Drawers/Drawers.h"
#ifdef WIP
#include "widgets/Drawers/ShaderDrawers.h"
#include "graphic/Effects/EffectActors.h"
#endif
#include "widgets/Animations/Animations.h"
#include "widgets/Drawers/ProxyDrawers.h"
#include "widgets/Drawers/CompositeDrawers.h"
#include "widgets/Drawers/DestinationDrawers.h"
#include "widgets/Layouters/ScriptLayouters.h"
#include "graphic/Particles/Particles.h"
#include "scene/Script/Event.h"
#include "script/ScriptClassParser.h"




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
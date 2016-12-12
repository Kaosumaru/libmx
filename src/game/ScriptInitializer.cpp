


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
#include "widgets/WidgetScriptBinding.h"

#include "devices/Keyboard.h"
#include "application/window.h"
#include "SDL_keycode.h"


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

	MX::Widgets::WidgetScriptBinding::Init();



}

void MX::ScriptInitializer::RegisterShortcuts()
{
#ifndef MX_GAME_RELEASE
    MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_F5].static_connect( []() 
    {
        ScriptInitializer::ReloadScripts();
    });
    MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_F6].static_connect( []() 
    {
        ScriptInitializer::ReloadScripts(true);
    });
    MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_F1].static_connect( []() 
    {
        MX::Widgets::Widget::drawDebug = !MX::Widgets::Widget::drawDebug;
    });
#endif
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
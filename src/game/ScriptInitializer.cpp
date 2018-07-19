


#include "ScriptInitializer.h"

#include "game/resources/Resources.h"
#include "widgets/drawers/Drawers.h"
#ifdef WIP
#include "graphic/effects/EffectActors.h"
#endif
#include "widgets/animations/Animations.h"
#include "widgets/drawers/ProxyDrawers.h"
#include "widgets/drawers/CompositeDrawers.h"
#include "widgets/drawers/ShaderDrawers.h"
#include "widgets/drawers/DestinationDrawers.h"
#include "widgets/layouters/ScriptLayouters.h"
#include "graphic/particles/Particles.h"
#include "scene/script/Event.h"
#include "script/ScriptClassParser.h"
#include "widgets/WidgetScriptBinding.h"

#include "devices/Keyboard.h"
#include "application/Window.h"
#include "SDL_keycode.h"


using namespace MX;

using namespace std;

void MX::ScriptInitializer::Init()
{
	

	MX::ScriptableSpriteActorInit::Init();
	MX::EventInit::Init();

#ifdef WIP
	MX::Graphic::EffectActorsInit::Init();
#endif

	MX::Graphic::ParticleInit::Init();
	MX::Widgets::DrawersInit::Init();
	MX::Widgets::ProxyDrawersInit::Init();
	MX::Widgets::CompositeDrawersInit::Init();
	MX::Widgets::ShaderDrawersInit::Init();
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
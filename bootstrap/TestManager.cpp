#include "TestManager.h"
#include "Game/Resources/Paths.h"
#include "Game/Resources/Resources.h"
#include "Script/Script.h"
#include "Script/ScriptObject.h"
#include "Application/Window.h"



#include "Devices/Mouse.h"
#include "Devices/Keyboard.h"
#include "Application/Window.h"
#include "game/ScriptInitializer.h"

#include "Script/ScriptClassParser.h"

using namespace MX;
using namespace std;

TestManager::TestManager() : DisplaySceneTimer(MX::Window::current().size())
{
    //_cheats = CreateCheats();
	_visibility = -1;

	MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_SPACE].connect(std::bind(&TestManager::reloadScripts, this));
	MX::Window::current().keyboard()->on_specific_key_down[SDL_SCANCODE_R].connect(std::bind(&TestManager::clearReloadScripts, this));

	reloadScripts();

    Context<BaseGraphicScene>::SetCurrent(*this);
}

void TestManager::AddSomething()
{
#if 0
	std::shared_ptr<SpriteActor> actor = Script::valueOf("Game.Resources.Explosion.ExplosionTest").to_object<SpriteActor>();
	if (!actor)
		return;
	auto clone = actor->clone();
	clone->geometry.position = point;
#endif
}


void TestManager::clearReloadScripts()
{
#ifdef WIP
	MX::Resources::get().Clear();
#endif
	reloadScripts();
}

void TestManager::reloadScripts()
{
    ScriptInitializer::ReloadScripts();

    Clear();

    ScriptObjectString script("Test.Scene");
    std::list<SpriteActorPtr> arr;
    script.load_property(arr, "Actors");

    for (auto &actor : arr)
        AddActor(actor);

}




void TestManager::Draw(float x, float y)
{
	DisplaySceneTimer::Draw(x, y);
}




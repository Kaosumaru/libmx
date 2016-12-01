#include "ControlScheme.h"
#include "Collision/Area/EventsToCollisions.h"
#include "Application/Window.h"
#include "Scene/Sprites/SpriteScene.h"
#include "Widgets/Widget.h"

#include "Collision/Area/Area.h"
#include "Devices/Keyboard.h"

using namespace MX;
using namespace MX::Game;

class KeyAction : public Action
{
public:
    KeyAction(ControlScheme* scheme, int keycode) : Action(scheme)
    {
        MX::Window::current().keyboard()->on_specific_key_up[keycode].connect([&]() { SetState(false); });
        MX::Window::current().keyboard()->on_specific_key_down[keycode].connect([&]() { SetState(true); });
    } 
};

void ControlScheme::Run()
{
    _queue.Run();
    for (auto &action : _actions)
        action->Run();
}

namespace MX
{
namespace Game
{
    Action::pointer actionForKey(int keycode)
    {
        return std::make_shared<KeyAction>(nullptr, keycode);
    }
}
}

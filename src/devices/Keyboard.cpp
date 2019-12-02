#include "Keyboard.h"
#include "EventWatcher.h"
#include <string>
using namespace MX;

Keyboard::Keyboard()
{
}

class SDLKeyboard : public Keyboard, public SDLEventWatcher
{
public:
    SDLKeyboard(Window* window)
    {
    }

    void OnInactive()
    {
        for (int i = 0; i < MaxKey; i++)
            if (_pressed_keys[i])
                OnKeycodeUp(i);
    }

    void OnKeyDown(int keycode)
    {
        _pressed_keys[keycode] = true;
        on_key_down(keycode);
        on_specific_key_down[keycode]();
    }

    void OnKeycodeUp(int keycode)
    {
        if (!_pressed_keys[keycode])
            return;
        _pressed_keys[keycode] = false;
        on_key_up(keycode);
        on_specific_key_up[keycode]();
    }

    void OnEvent(SDL_Event& event) override
    {
        if (event.type == SDL_KEYDOWN)
        {
            OnKeyDown(event.key.keysym.scancode);
        }
        else if (event.type == SDL_KEYUP)
        {
            OnKeycodeUp(event.key.keysym.scancode);
        }
        else if (event.type == SDL_TEXTINPUT)
        {
            std::string txt { event.text.text };
            if (txt.size() > 0)
                on_char(txt[0]);
        }
    }
};

Keyboard::pointer Keyboard::CreateForWindow(Window* window)
{
    return std::make_shared<SDLKeyboard>(window);
}
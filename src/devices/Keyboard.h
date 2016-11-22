#pragma once
#include<memory>
#include "Utils/Utils.h"
#include "Utils/Signal.h"
#include "SDL_keycode.h"

namespace MX
{
	class Window;

	class Keyboard : public shared_ptr_init<Keyboard>
	{
	public:
		Keyboard();

		static pointer CreateForWindow(Window* window);

		const static int MaxKey = (int)SDL_NUM_SCANCODES;
		Signal<void(int keycode)> on_key_up;
		Signal<void(int keycode)> on_key_down;

		Signal<void()> on_specific_key_up[MaxKey];
		Signal<void()> on_specific_key_down[MaxKey];

		Signal<void(int unichar, int keycode, unsigned modifiers, bool repeated)> on_char;

		bool key(int keycode) { return _pressed_keys[keycode]; }

	protected:
		bool _pressed_keys[MaxKey];

	};

}

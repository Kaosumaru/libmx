#pragma once
#include<memory>
#include "utils/Utils.h"
#include "utils/Signal.h"
#include "glm/vec2.hpp"

namespace MX
{
	class Window;

	class Mouse : public shared_ptr_init<Mouse>
	{
	public:
		const static unsigned buttons = 5;
		Mouse();

		static pointer CreateForWindow(Window* window);

		float mouseX();
		float mouseY();
		int mouseWheel();
		const glm::vec2& position();
		bool button[buttons] = {};


		Signal<void(const glm::vec2& position)> on_mouse_enter; //warning, NYI
		Signal<void(const glm::vec2& position)> on_mouse_leave; //warning, NYI

		Signal<void(const glm::vec2& position, const glm::ivec2& delta)> on_mouse_move;

		Signal<void(const glm::vec2& position,int button)> on_mouse_button_down;
		Signal<void(const glm::vec2& position,int button)> on_mouse_button_up;

		Signal<void(const glm::vec2& position)> on_specific_button_up[buttons];
		Signal<void(const glm::vec2& position)> on_specific_button_down[buttons];



	protected:
		glm::vec2 _position = {-1, -1};
		int _wheel = 0;
	};

}

#include "Scene/Script/Command.h"
#include "Scene/Script/CommonCommands.h"
#include "Widgets/Widget.h"
#include "glm/gtx/transform.hpp"

namespace MX
{
	namespace Widgets
	{
		namespace Animation
		{
			class Translate : public EasingWaitCommand
			{
			public:
				Translate(const glm::vec2 &p, float time) : EasingWaitCommand(time), _p(p)
				{

				}
			protected:
				void OnWait() override
				{
					auto &w = Context<Widget>::current();
					auto dest = lerp({ 0.0f,0.0f }, _p, percent());

					glm::mat4 t;
					t = glm::translate(t, { dest.x, dest.y, 0.0f });
					w.SetTransform(t);
				}

				glm::vec2 _p;
			};
		}

		class WidgetAnimationsInit
		{
		public:
			static void Init();
		};

	}
}

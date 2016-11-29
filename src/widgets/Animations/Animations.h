#ifndef MXANIMATIONS
#define MXANIMATIONS
#include "Scene/Script/MXCommand.h"
#include "Scene/Script/MXCommonCommands.h"
#include "Widgets/MXWidget.h"

namespace bs2 = boost::signals2;

namespace MX
{
	namespace Widgets
	{
		namespace Animation
		{
			class Translate : public EasingWaitCommand
			{
			public:
				Translate(const MX::Vector2 &p, float time) : EasingWaitCommand(time), _p(p)
				{

				}
			protected:
				void OnWait() override
				{
					auto &w = Context<Widget>::current();
					auto dest = Vector2::lerp({ 0.0f,0.0f }, _p, percent());

					glm::mat4 t;
					t = glm::translate(t, { dest.x, dest.y, 0.0f });
					w.SetTransform(t);
				}

				MX::Vector2 _p;
			};
		}

		class WidgetAnimationsInit
		{
		public:
			static void Init();
		};

	}
}


#endif

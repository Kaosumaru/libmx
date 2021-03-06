#include "glm/gtx/transform.hpp"
#include "scene/script/Command.h"
#include "scene/script/CommonCommands.h"
#include "widgets/Widget.h"

namespace MX
{
namespace Widgets
{
    namespace Animation
    {
        class Translate : public EasingWaitCommand
        {
        public:
            Translate(const glm::vec2& p, float time)
                : EasingWaitCommand(time)
                , _p(p)
            {
            }

        protected:
            void OnWait() override
            {
                auto& w = Context<Widget>::current();
                auto dest = lerp({ 0.0f, 0.0f }, _p, percent());

                glm::mat4 t;
                t = glm::translate(t, { dest.x, dest.y, 0.0f });
                w.SetTransform(t);
            }

            glm::vec2 _p;
        };

        class WidgetMoveToCommand : public EasingWaitCommand
        {
        public:
            WidgetMoveToCommand(const glm::vec2& p, float time)
                : EasingWaitCommand(time)
                , _vec(p)
            {
            }

        protected:
            void OnWait() override
            {
                auto& w = Context<Widget>::current();

                if (_start)
                {
                    _vecOrig = w.position();
                    _start = false;
                }

                w.SetPosition(MX::lerp(_vecOrig, _vec, percent()));
            }

            bool _start = true;
            glm::vec2 _vecOrig;
            glm::vec2 _vec;
        };
    }

    class WidgetAnimationsInit
    {
    public:
        static void Init();
    };

}
}

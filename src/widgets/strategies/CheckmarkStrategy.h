#ifndef MXCHECKMARKSTRATEGY
#define MXCHECKMARKSTRATEGY
#include "Strategy.h"
#include "devices/Touches.h"

namespace MX
{
namespace Widgets
{

    class CheckmarkGroup : public MX::SignalTrackable
    {
    public:
        CheckmarkGroup() { }
        ~CheckmarkGroup() { }

        void AddWidget(const std::shared_ptr<ButtonWidget>& widget);

    protected:
        void onWidgetSelected(ButtonWidget* widget);

        std::list<std::shared_ptr<ButtonWidget>> _widgets;
    };

    namespace Strategy
    {
        class Checkmark : public Strategy
        {
        public:
            Checkmark();

            ButtonWidget& buttonWidget();

        protected:
            void OnInit();

            void OnTouchBeginCollision(const Collision::Shape::pointer& shape);
            void OnMouseButtonCollision(const Collision::Shape::pointer& shape);

            Touch::pointer _touch;
            ButtonWidget* _buttonWidget;
        };
    }

}
}

#endif

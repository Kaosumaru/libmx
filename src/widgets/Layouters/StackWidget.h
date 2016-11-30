#ifndef MXSTACKWIDGET
#define MXSTACKWIDGET
#include "Widgets/Widget.h"
namespace MX
{
namespace Widgets
{



class StackWidget : public Widget
{
public:
	StackWidget();


	void PushWidget(const std::shared_ptr<Widget> &widget);
	void PopWidget();

	void ReplaceTopWidget(const std::shared_ptr<Widget> &widget);

	static StackWidget* stack_of(Widget *widget);

	unsigned stackSize() { return _stackSize; }

	const std::shared_ptr<Widget>& top() { return _top; }
	Signal<void(const std::shared_ptr<Widget> &)> onTopWidgetChanged;
protected:
	void OnAreaInsideChanged() override;
	void AddWidget(const std::shared_ptr<Widget> &widget) override { return Widget::AddWidget(widget);  }
	
	std::shared_ptr<Widget> _top;
	unsigned _stackSize = 0;
};



}
typedef std::shared_ptr<Widgets::StackWidget> StackWidgetPtr;


}

#endif

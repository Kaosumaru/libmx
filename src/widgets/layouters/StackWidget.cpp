#include "StackWidget.h"
#include "application/Window.h"
#include "Game/Resources/Resources.h"
#include <iostream>

using namespace MX;
using namespace MX::Widgets;

StackWidget::StackWidget()
{

}


void StackWidget::PushWidget(const std::shared_ptr<Widget> &widget)
{

	{
		auto guard = _subWidgets.guard();
		auto it = _subWidgets.begin();
		while (it != _subWidgets.end())
		{
			(*it)->ChangeVisibility(1);
			_subWidgets.temp_advance(it);
		}
	}

	_stackSize++;
	widget->SetSize(_width, _height);
	AddWidget(widget);
	_top = widget;
	onTopWidgetChanged(widget);
}

void StackWidget::PopWidget()
{
	if (_stackSize == 0)
	{
		assert(false);
		return;
	}
		
	std::shared_ptr<Widget> last;
	std::shared_ptr<Widget> prev;
	{
		auto guard = _subWidgets.guard();
		auto it = _subWidgets.begin();
		while (it != _subWidgets.end())
		{
			prev = last;
			last = (*it);
			(*it)->ChangeVisibility(-1);
			_subWidgets.temp_advance(it);
		}
	}
	if (last)
	{
		last->ChangeVisibility(1);
		last->Unlink();
	}
		

	_stackSize--;
	_top = prev;
	onTopWidgetChanged(prev);
}


void StackWidget::ReplaceTopWidget(const std::shared_ptr<Widget> &widget)
{
	if (stackSize() == 0)
	{
		PushWidget(widget);
		return;
	}

	std::shared_ptr<Widget> last;
	std::shared_ptr<Widget> prev;
	{
		auto guard = _subWidgets.guard();
		auto it = _subWidgets.begin();
		while (it != _subWidgets.end())
		{
			last = (*it);
			_subWidgets.temp_advance(it);
		}
	}

	if (last)
		last->Unlink();

	widget->SetSize(_width, _height);
	AddWidget(widget);
	_top = widget;
	onTopWidgetChanged(widget);
}

void StackWidget::OnAreaInsideChanged()
{
	Widget::OnAreaInsideChanged();

	{
		auto guard = _subWidgets.guard();
		auto it = _subWidgets.begin();
		while (it != _subWidgets.end())
		{
			(*it)->SetSize(_width, _height);
			_subWidgets.temp_advance(it);
		}
	}
}

StackWidget* StackWidget::stack_of(Widget *widget)
{
	SpriteActor *act = widget;
	while (act->linked())
	{
		act = &(act->sprite_scene());
		auto stack = dynamic_cast<StackWidget*>(act);
		if (stack)
			return stack;
	}
	return nullptr;
}
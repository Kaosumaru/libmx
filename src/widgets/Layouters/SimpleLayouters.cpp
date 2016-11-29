#include "MXSimpleLayouters.h"
#include "Application/MXWindow.h"
#include "Game/Resources/MXResources.h"

#include "../Drawers/MXDrawer.h"
#include <iostream>

using namespace MX::Widgets;

Layouter::Layouter()
{
	_inner_margin = 0;
}

void Layouter::SetMargins(const MX::Margins &margins)
{
	_margins = margins;
	LayoutAllItems();
}

void Layouter::SetInnerMargin(int inner)
{
	_inner_margin = inner;
	LayoutAllItems();
}

void Layouter::SetSize(float width, float height)
{
	Widget::current().SetSize(width, height);
}


//TODO relayout on subwidget size change, and subwidget unlink
LayouterWidget::LayouterWidget()
{
}
void LayouterWidget::AddWidget(const std::shared_ptr<Widget> &widget)
{
	auto guard = Context<Widget>::Lock(this);
	layouter()->LayoutNewItem(widget);
	Widget::AddWidget(widget);
}



void LayouterWidget::SetLayouter(const Layouter::pointer &layouter)
{
	_layouter = layouter;
	auto guard = Context<Widget>::Lock(this);
	_layouter->LayoutAllItems();
}

Layouter::pointer &LayouterWidget::layouter()
{
	return _layouter;
}



void HorizontalLayouter::LayoutNewItem(const std::shared_ptr<MX::Widgets::Widget> &widget)
{
	unsigned pos;
	if (!Widget::current().widgets().empty())
	{
		auto relativeX = Widget::current().widgets().back()->relativePosition().x;
		auto width = Widget::current().widgets().back()->Width();
		pos = relativeX + width + _inner_margin;
	}
	else
		pos = _margins.left;
	widget->SetPosition(pos, _margins.top);
	SetSize(pos + widget->Width() + _margins.right, Widget::current().Height() < widget->Height() + _margins.vMargins() ? widget->Height() + _margins.vMargins() : Widget::current().Height());
}
void HorizontalLayouter::LayoutAllItems()
{
	//TODO
}

void HorizontalLayouter::ItemRemoved(const std::shared_ptr<MX::Widgets::Widget> &widget)
{
	//TODO
}
void HorizontalLayouter::ItemResized(const std::shared_ptr<MX::Widgets::Widget> &widget)
{
	//TODO
}

void VerticalLayouter::LayoutNewItem(const std::shared_ptr<MX::Widgets::Widget> &widget)
{
	unsigned pos;
	if (!Widget::current().widgets().empty())
	{
		auto relativeY = Widget::current().widgets().back()->relativePosition().y;
		auto height = Widget::current().widgets().back()->Height();
		pos = relativeY + height + _inner_margin;
	}
	else
		pos = _margins.top;
	widget->SetPosition(_margins.left, pos);
	SetSize(Widget::current().Width() < widget->Width() + _margins.hMargins() ? widget->Width() + _margins.hMargins() : Widget::current().Width(), pos + widget->Height() + _margins.bottom);
}
void VerticalLayouter::LayoutAllItems()
{
	//TODO
}

void VerticalLayouter::ItemRemoved(const std::shared_ptr<MX::Widgets::Widget> &widget)
{
	//TODO
}
void VerticalLayouter::ItemResized(const std::shared_ptr<MX::Widgets::Widget> &widget)
{
	//TODO
}



VerticalLayouterWidget::VerticalLayouterWidget(int inner)
{
	SetLayouter(MX::make_shared<VerticalLayouter>());
	layouter()->SetInnerMargin(inner);
}


HorizontalLayouterWidget::HorizontalLayouterWidget(int inner)
{
	SetLayouter(MX::make_shared<HorizontalLayouter>());
	layouter()->SetInnerMargin(inner);
}







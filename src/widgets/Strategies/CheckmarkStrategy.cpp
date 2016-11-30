#include "CheckmarkStrategy.h"
#include "../Widget.h"
#include "Application/Window.h"
#include "Game/Resources/Resources.h"
#include <iostream>
using namespace MX;
using namespace MX::Widgets;

void CheckmarkGroup::AddWidget(const std::shared_ptr<ButtonWidget>& widget)
{
	_widgets.push_back(widget);
	widget->onTouched.connect(std::bind(&CheckmarkGroup::onWidgetSelected, this, widget.get()));
}


void CheckmarkGroup::onWidgetSelected(ButtonWidget* widget)
{
	for (auto &w : _widgets)
	{
		w->SetSelected(w.get() == widget);
	}
}
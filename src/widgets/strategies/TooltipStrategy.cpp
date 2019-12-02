#include "TooltipStrategy.h"
#include "../Widget.h"
#include "application/Window.h"
#include "collision/area/EventsToCollisions.h"
#include "game/resources/Resources.h"
#include "widgets/Label.h"
#include <iostream>

using namespace MX;
using namespace MX::Widgets;
using namespace MX::Strategies;

Tooltip::Tooltip(const std::string& drawerPath)
    : _drawerPath(drawerPath)
{
}

Tooltip::Tooltip(const std::wstring& text, const std::string& drawerPath)
    : Tooltip(drawerPath)
{
    _tooltipText = text;
}

void Tooltip::onHoverIn()
{
    Show();
}

void Tooltip::onHoverOut()
{
    Hide();
}

void Tooltip::OnInit()
{
    //auto buttonWidget = static_cast<ButtonWidget*>(_widget);
    //buttonWidget->onHoverIn.connect(std::bind(&Tooltip::onHoverIn, this));
    //buttonWidget->onHoverOut.connect(std::bind(&Tooltip::onHoverOut, this));
}

void Tooltip::onDraw()
{
    if (_tooltipWidget)
        _tooltipWidget->Draw(_position.x, _position.y);
}

void Tooltip::onShow()
{
    auto buttonDrawer = MX::Widgets::DrawerManager::drawer(_drawerPath);

    auto text = _tooltipText;
    if (_tooltipText.empty())
    {
        auto labelWidget = dynamic_cast<MX::Widgets::Label*>(_widget);
        if (labelWidget)
            text = labelWidget->text();
    }

    auto txt = std::make_shared<MX::Widgets::Label>(text);
    txt->SetDrawer(buttonDrawer);
    txt->ChangeVisibility(-1);

    _position = MX::Window::current().mouse()->position();
    _position.y += 32;

    _tooltipWidget = txt;
}

void Tooltip::onHide()
{
    _tooltipWidget = nullptr;
}

bool Tooltip::Run()
{
    auto buttonWidget = static_cast<ButtonWidget*>(_widget);
    bool hover = buttonWidget->hover();

    if (hover != _wasHover)
    {
        _wasHover = hover;

        if (hover)
        {
            _conn = std::make_shared<int>();
            FunctorsQueue::current().planWeakFunctor(
                _showTime, [&]() { Show(); }, _conn);
        }
        else
        {
            _conn.reset();
            Hide();
        }
    }

#if 0
	if (buttonWidget->pressed())
	{
		_conn.reset();
		Hide();
	}
#endif

    if (_tooltipWidget)
        _tooltipWidget->Run();

    return true;
}
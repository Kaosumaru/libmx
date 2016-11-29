#include "MXScriptDrawerLayouters.h"
#include "Application/MXWindow.h"
#include "Game/Resources/MXResources.h"
#include <iostream>
#include "Script/MXPropertyLoaders.h"
#include "Script/MXScriptClassParser.h"
#include "MXScriptLayouters.h"

using namespace MX::Widgets;


class ScriptLayouterDrawer : public ScriptLayouterBase
{
public:
	friend class LayoutDrawer;

	ScriptLayouterDrawer(MX::LScriptObject& script) : ScriptLayouterBase(script)
	{
		script.load_property(_children, "Children");
	}

	void LayoutWidget(const std::string& name, const std::shared_ptr<Widget> &widget) override
	{
		Context<ScriptLayouterWidget>::current().InvalidateLayout();
	}


	void RelayoutAllWidgets(const NamedWidgetsMap& named_widgets)
	{
		//TODO because layouter is reused by multiple widgets, that may happen in future
		if (_widgetMap)
		{
			assert(false);
			return;
		}
		FindDrawersForAllWidgets(named_widgets);


		_widgetMap = &named_widgets;
		DoLayout();
		_widgetMap = nullptr;
	}

	void DrawBackground() override
	{
		for (auto &c : _children)
			c->DrawBackground();
	}

protected:
	auto WidgetsWithName(const std::string& name)
	{
		return _widgetMap->equal_range(name);
	}

	void DoLayout()
	{
		auto &w = Context<Widget>::current();
		auto r = MX::Rectangle::fromWH(0.0f, 0.0f, w.Width() - w.margins().hMargins(), w.Height() - w.margins().vMargins());
		auto guard2 = Context<Drawer::Destination>::Lock(Drawer::Destination(r));
		auto guard = Context<ScriptLayouterDrawer>::Lock(this);
		for (auto &c : _children)
			c->DrawBackground();
	}


	const NamedWidgetsMap *_widgetMap = nullptr;
	std::vector<Drawer::pointer> _children;
};


class LayoutDrawer : public Drawer
{
public:
	enum class Mode
	{
		Normal, 
		Center,
		NumPos
	};

	LayoutDrawer(MX::LScriptObject& script) : Drawer(script)
	{
		if (!script.load_property(_name, "n"))
			script.load_property(_name, "name");

		std::string mode;
		if (script.load_property(mode, "mode"))
		{
			if (mode == "center")
				_mode = Mode::Center;
		}

		if (script.load_property(_p, "p"))
		{
			_mode = Mode::NumPos;
		}
	}

	void DrawBackground() override
	{
		if (!Context<ScriptLayouterDrawer>::isCurrent())
			return;
		auto &l = Context<ScriptLayouterDrawer>::current();
		auto r = l.WidgetsWithName(_name);

		for (auto it = r.first; it != r.second; it++)
		{
			auto &w = it->second;
			LayoutWidget(w);
		}
	}

protected:
	void LayoutWidget(const std::shared_ptr<Widget> &widget)
	{
		auto &current = Drawer::Destination::current();

		if (_mode == Mode::Normal)
		{
			widget->SetPositionRect(current.rectangle);
			return;
		}

		if (_mode == Mode::Center)
		{
			auto minSize = widget->minDimensions();
			MX::Rectangle r(0.0f, 0.0f, minSize.x, minSize.y);
			r.CenterIn(current.rectangle);

			widget->SetPositionRect(r);
			return;
		}

		if (_mode == Mode::NumPos)
		{
			auto minSize = widget->minDimensions();
			MX::Rectangle r(0.0f, 0.0f, minSize.x, minSize.y);
			r.NumLayoutIn(current.rectangle, _p);

			widget->SetPositionRect(r);
			return;
		}
		
	}

	std::string _name;
	Mode _mode = Mode::Normal;
	int  _p = -1;
};



void ScriptDrawerLayouterInit::Init()
{
	ScriptClassParser::AddCreator(L"Layouter.Drawer", new OutsideScriptClassCreatorContructor<ScriptLayouterDrawer>());

	ScriptClassParser::AddCreator(L"Layout", new OutsideScriptClassCreatorContructor<LayoutDrawer>());
}

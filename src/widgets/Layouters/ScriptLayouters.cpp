#include "ScriptLayouters.h"
#include "application/Window.h"
#include "Game/Resources/Resources.h"
#include <iostream>
#include "script/PropertyLoaders.h"
#include "script/ScriptClassParser.h"
#include "ScriptDrawerLayouters.h"
#include "ScriptLayoutersCommon.h"
#include "WidgetFactory.h"
#include "script/PropertyLoaders.h"

using namespace MX::Widgets;

void ScriptLayouter::ChangeInnerDimension(const glm::vec2& size)
{
	auto &widget = Context<ScriptLayouterWidget>::current();

	auto &margins = widget.margins();

	widget.SetSize(size.x + margins.hMargins(), size.y + margins.vMargins());
	widget.NotifyParentAboutSizeUpdate();
}

ScriptLayouterBase::ScriptLayouterBase(MX::LScriptObject& script)
{
	script.load_property(_customWidgets, "Widgets");
	script.load_property(_drawer, "Drawer");

	{
		using Pair = std::pair<std::shared_ptr<Drawer>, std::vector<std::string>>;
		std::vector<Pair> drawers;
		script.load_property(drawers, "Drawers");

		for (auto &pair : drawers)
		{
			for (auto &name : pair.second)
				_drawers[name] = pair.first;
		}
	}

	{
		using Pair = std::pair<Scriptable::Value::pointer, std::vector<std::string>>;
		std::vector<Pair> layouters;
		script.load_property(layouters, "Layouters");

		for (auto &pair : layouters)
		{
			std::string path;
			Scriptable::Value::pointer root = pair.first;
			while (root->pointer_to_next())
				root = root->pointer_to_next();
			path = root->fullPath();

			for (auto &name : pair.second)
				_layouters[name] = path;
		}
	}

}

void ScriptLayouterBase::OnLinkedToWidget()
{
	auto &widget = Context<ScriptLayouterWidget>::current();

	if (_drawer)
		widget.SetDrawer(_drawer);

	for (auto& data : _customWidgets)
	{
		auto w = data.factory ? data.factory->Create() : std::make_shared<Widget>();
		widget.AddLayouterWidget(data.name, w);
	}
		
}


void ScriptLayouterBase::LayoutWidget(const std::string& name, const std::shared_ptr<Widget> &widget)
{
	FindDrawerForWidget(name, widget);
}

void ScriptLayouterBase::FindDrawerForWidget(const std::string& name, const std::shared_ptr<Widget> &widget)
{
	{
		auto it = _drawers.find(name);
		if (it != _drawers.end())
			widget->SetDrawer(it->second);
	}

	{
		auto it = _layouters.find(name);
		if (it != _layouters.end())
		{
			auto layouterWidget = std::dynamic_pointer_cast<ScriptLayouterWidget>(widget);
			if (layouterWidget)
				layouterWidget->SetLayouter(it->second);
		}
			
	}
}


class ScriptLayouterManagerLayouterProxy : public ScriptLayouter
{
public:
	ScriptLayouterManagerLayouterProxy(const std::string& objectPath)
	{
		_objectPath = objectPath;
		loadLayouter();
	}

	void LayoutWidget(const std::string& name, const std::shared_ptr<Widget> &widget) override { return _layouter->LayoutWidget(name, widget); };

	int version() override
	{
		return _version;
	}

	void loadLayouter()
	{
		_layouter = MX::Script::valueOf(_objectPath).to_object<ScriptLayouter>();
		_version++;
		assert(_layouter);
	}

	void OnLinkedToWidget() override
	{
		return _layouter->OnLinkedToWidget();
	}

	void DrawBackground() override
	{
		return _layouter->DrawBackground();
	}

	void RelayoutAllWidgets(const NamedWidgetsMap& named_widgets) override
	{
		_layouter->RelayoutAllWidgets(named_widgets);
	}

	void clipSize(float &width, float &height) override
	{
		_layouter->clipSize(width, height);
	}


protected:
	int _version = 0;
	ScriptLayouter::pointer _layouter;
	std::string _objectPath;
};


ScriptLayouterManager::ScriptLayouterManager()
{
#ifdef _DEBUG
	Script::onParsed.connect([&]() { onReload(); });
#endif
}

ScriptLayouter::pointer ScriptLayouterManager::getLayouter(const std::string& objectPath)
{
	auto &layouter = _layouters[objectPath];
	if (layouter)
		return layouter;

#ifdef _DEBUG
	layouter = std::make_shared<ScriptLayouterManagerLayouterProxy>(objectPath);
#else
	layouter = Script::valueOf(objectPath).to_object<ScriptLayouter>();
#endif

	

	assert(layouter);
	return layouter;
}



void ScriptLayouterManager::Deinitialize()
{
	_layouters.clear();
}


void ScriptLayouterManager::onReload()
{
	for (auto& pair : _layouters)
	{
		auto layouter = std::static_pointer_cast<ScriptLayouterManagerLayouterProxy>(pair.second);
		layouter->loadLayouter();
	}
}






ScriptLayouterWidget::ScriptLayouterWidget()
{
}


void ScriptLayouterWidget::DrawBackground()
{
	Widget::DrawBackground();



    if (_layouter)
        _layouter->DrawBackground();

}


void ScriptLayouterWidget::Run()
{
	if (_layouter)
	{
		auto guard = Context<Widget>::Lock(this);
		auto guard2 = Context<ScriptLayouterWidget>::Lock(this);

#ifdef _DEBUG
		if (_layouterVersion != _layouter->version())
		{
			auto layouter = _layouter;
			_layouter = nullptr;
			SetLayouter(layouter);
		}
#endif
		InternalsOnRun();
		RelayoutIfNeeded();
		ChildrenOnRun();
		RelayoutIfNeeded(); // childs could changed their size by themself, we need to relayout them

		return;
	}

	Widget::Run();
}


void ScriptLayouterWidget::RelayoutIfNeeded()
{
	if (!_layoutDirty || !_layouter)
		return;
	_layoutDirty = false;
	relayoutNamedWidgets();
}


void ScriptLayouterWidget::SetDefaultWidgetName(const std::string& name)
{
	_defaultWidgetName = name;
}

void ScriptLayouterWidget::AddWidget(const std::shared_ptr<Widget> &widget)
{
	auto guard = Context<Widget>::Lock(this);
	//layouter()->LayoutNewItem(widget);
	if (!_defaultWidgetName.empty())
	{
		AddNamedWidget(_defaultWidgetName, widget);
		return;
	}

	Widget::AddWidget(widget);
}

ScriptLayouterWidget::NamedWidgetsMap::iterator ScriptLayouterWidget::AddNamedWidget(const std::string& name, const std::shared_ptr<Widget> &widget)
{
	auto guard = Context<Widget>::Lock(this);
	Widget::AddWidget(widget);

	auto d = dimensionsInside();

	auto guard2 = Context<ScriptLayouterWidget>::Lock(this);
	auto guard3 = Context<Drawer::Destination>::Lock(Drawer::Destination(Rectangle::fromWH(0.0, 0.0, d.x, d.y)));
	if (_layouter && !_layoutDirty)
		_layouter->LayoutWidget(name, widget);

	return _namedWidgets.emplace(name, widget);
}

void ScriptLayouterWidget::AddLayouterWidget(const std::string& name, const std::shared_ptr<Widget> &widget)
{
	auto it = AddNamedWidget(name, widget);
	_layouterWidgets.push_back(it);
}

void ScriptLayouterWidget::RemoveLayouterWidgets()
{
	std::vector<NamedWidgetsMap::iterator> layouterWidgets;
	std::swap(layouterWidgets, _layouterWidgets);

	for (auto &it : layouterWidgets)
		it->second->Unlink();

}

void ScriptLayouterWidget::Clear()
{
	RemoveLayouterWidgets();
	_namedWidgets.clear();
	Widget::Clear();
}


void ScriptLayouterWidget::SetLayouter(const ScriptLayouter::pointer &layouter)
{
	if (_layouter == layouter)
		return;
	RemoveLayouterWidgets();
	_layouter = layouter;

	InvalidateLayout();
	//relayoutNamedWidgets();


	if (_layouter)
	{ 
		//this potentially add new widgets
		auto d = dimensionsInside();

		auto guard = Context<Widget>::Lock(this);
		auto guard2 = Context<ScriptLayouterWidget>::Lock(this);
		auto guard3 = Context<Drawer::Destination>::Lock(Drawer::Destination(Rectangle::fromWH(0.0, 0.0, d.x, d.y)));

		_layouter->OnLinkedToWidget();
#ifdef _DEBUG
		_layouterVersion = layouter->version();
#endif
	}
		
	
}


void ScriptLayouterWidget::onChildSizeChanged()
{
	InvalidateLayout();
}

void ScriptLayouterWidget::onChildRemoved(Widget& child)
{
	//remove from named, if exists
	{
		auto iter = _namedWidgets.begin();
		for (; iter != _namedWidgets.end(); ++iter)
		{
			if (iter->second.get() != &child)
				continue;
			_namedWidgets.erase(iter);
			break;
		}
	}


	InvalidateLayout();
}

void ScriptLayouterWidget::SetLayouter(const std::string& objectPath)
{
	auto layouter = MX::Widgets::ScriptLayouterManager::layouter(objectPath);
	SetLayouter(layouter);
}


const ScriptLayouter::pointer &ScriptLayouterWidget::layouter()
{
	return _layouter;
}

void ScriptLayouterWidget::relayoutNamedWidgets()
{
	auto d = dimensionsInside();

	auto guard = Context<Widget>::Lock(this);
	auto guard2 = Context<ScriptLayouterWidget>::Lock(this);
	auto guard3 = Context<Drawer::Destination>::Lock(Drawer::Destination(Rectangle::fromWH(0.0, 0.0, d.x, d.y)));

	if (!_layouter)
		return;
	_layouter->RelayoutAllWidgets(_namedWidgets);
}


void ScriptLayouterWidget::OnAreaInsideChanged()
{
	Widget::OnAreaInsideChanged();
	InvalidateLayout();
	//relayoutNamedWidgets();
}

void ScriptLayouterWidget::clipSize(float &width, float &height)
{
	auto guard = Context<Widget>::Lock(this);
	auto guard2 = Context<ScriptLayouterWidget>::Lock(this);
	if (_drawer)
		_drawer->clipSize(width, height);
	if (_layouter)
		_layouter->clipSize(width, height);
}




class LayouterRectangle
{
public:
	LayouterRectangle()
	{
		_rectangle.x2 = _rectangle.y2 = -1.0f;
		memset(_coordAbsolute, 0, sizeof(_coordAbsolute));
	}

	Drawer::Destination destination()
	{
		if (!Drawer::Destination::isCurrent())
			return Drawer::Destination();

		auto &currentD = Drawer::Destination::current();

		Drawer::Destination dest(_rectangle);

		auto minus = [](float &number, float max) { number = number < 0.0f ? max + number : number; };

		float wh[] = { currentD.width(), currentD.height(), currentD.width(), currentD.height() };

		for (int i = 0; i < 4; i++)
		{
			if (!_coordAbsolute[i])
				minus(dest.rectangle.point(i), wh[i]);
		}

		if (_coordAbsolute[2])
			dest.rectangle.SetWidth(dest.rectangle.x2);

		if (_coordAbsolute[3])
			dest.rectangle.SetHeight(dest.rectangle.y2);


		dest.rectangle.Shift(currentD.rectangle.pointTopLeft());



		return dest;

	}

	std::pair<float&, bool&> coord(int i)
	{
		return std::make_pair(std::ref(_rectangle.point(i)), std::ref(_coordAbsolute[i]));
	}

	MX::Rectangle _rectangle;
	bool  _coordAbsolute[4];

};

/*
	Short recap:
	[0,0,-1,-1] - relative, full
	[0,0,[20,30]] or [0,0,[20],[30]] - size absolute
*/
namespace MX
{
	template<>
	struct PropertyLoader<LayouterRectangle>
	{
		using type = PropertyLoader_Standard;
		static bool load(LayouterRectangle& out, const Scriptable::Value::pointer& obj)
		{
			if (obj->size() == 0)
				return false;
			
			auto &o = *obj;
			
			int coord_index = 0;

			for (auto &coord_obj_ptr : obj->array())
			{
				auto &coord_obj = *coord_obj_ptr;

				//absolute
				if (coord_obj.size() > 0)
				{
					for (auto &sub : coord_obj.array())
					{
						auto &sub_coord_obj = *sub;
						auto coord = out.coord(coord_index);

						coord.first = sub_coord_obj;
						coord.second = true;

						coord_index++;
						if (coord_index == 4)
							return true;
					}


					//TODO
					continue;
				}
			
				//relative
				{
					auto coord = out.coord(coord_index);
					coord.first = coord_obj;
					coord.second = false;

					coord_index++;
					if (coord_index == 4)
						return true;
				}

			}

			return true;
		}
	};
}




class ScriptLayouterSimple : public ScriptLayouterBase
{
public:
	ScriptLayouterSimple(MX::LScriptObject& script) : ScriptLayouterBase(script)
	{	
		script.load_property(_layout, "Layout");
	}

	void LayoutWidget(const std::string& name, const std::shared_ptr<Widget> &widget) override
	{
		{
			auto it = _layout.find(name);
			if (it != _layout.end())
			{
				auto destination = it->second.destination();
				auto guard = destination.Use();

				widget->SetPositionRect(destination.rectangle);
			}
		}

		ScriptLayouterBase::LayoutWidget(name, widget);

	}
protected:
	std::map<std::string, LayouterRectangle> _layout;
};



void ScriptLayouterInit::Init()
{
	ScriptClassParser::AddCreator(L"Layouter.Simple", new OutsideScriptClassCreatorContructor<ScriptLayouterSimple>());

	WidgetFactoryInit::Init();
	ScriptDrawerLayouterInit::Init();
	ScriptLayouterCommonInit::Init();
}

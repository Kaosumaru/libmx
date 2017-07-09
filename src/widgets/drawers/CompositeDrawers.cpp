#include "CompositeDrawers.h"
#include "widgets/Widget.h"
#include "script/ScriptClassParser.h"
#include "utils/shapes/Rectangle.h"

using namespace MX;
using namespace MX::Widgets;



CompositeDrawer::CompositeDrawer(LScriptObject& script)
{
	{
		MX::Rectangle margins;
		//using Pair = std::pair<std::reference_wrapper<float>, std::reference_wrapper<float> >;
		using Pair = std::pair<float&, float&>;
		Pair marginsV{ margins.y1, margins.y2 };
		Pair marginsH{ margins.x1, margins.x2 };

		bool marginsPresent = false;

		marginsPresent |= script.load_property(margins, "Margins");
		marginsPresent |= script.load_property(marginsV, "Margins.V");
		marginsPresent |= script.load_property(marginsH, "Margins.H");
		

		if (marginsPresent)
			_optionalMargins = MX::Margins::fromRectangle(margins);
	}



	bool size = script.load_property(_defaultSize, "Size");
	
	if (!script.load_property(_minSize, "Size.Min") && size)
		_minSize = *_defaultSize;

	if (!script.load_property(_maxSize, "Size.Max") && size)
		_maxSize = *_defaultSize;

	script.load_property(_shapePolicy, "Shape");
	script.load_property(_properties, "Properties");
}

std::unique_ptr<ShapePolicy> CompositeDrawer::defaultShapePolicy() 
{ 
	if (_shapePolicy)
	{
		return _shapePolicy->clone();
	}
	return nullptr;
}

void CompositeDrawer::clipSize(float &width, float &height)
{
	if (width < _minSize.first)
		width = _minSize.first;
	if (height < _minSize.second)
		height = _minSize.second;

	if (_maxSize.first != -1.0f && width > _maxSize.first)
		width = _maxSize.first;
	if (_maxSize.second != -1.0f && height > _maxSize.second)
		height = _maxSize.second;
}

void CompositeDrawer::OnAssignedToWidget()
{
	auto &widget = Widget::current();

	for (auto& pair : _properties)
		widget.properties().SetValue(pair.first, pair.second);
}




CompositeLayouterDrawer::CompositeLayouterDrawer(LScriptObject& script) : CompositeDrawer(script)
{

	auto pchildren = script.property_object("Children");

	if (!pchildren)
		return;
	auto &children = *pchildren;


	auto size = children.size();
	_children.reserve(size);
	for (std::size_t i = 0; i < size; i++)
	{
		auto &child = children[i];


		if (child.size() == 0)
		{
			AddChild(child.to_object<Widgets::Drawer>());
			continue;
		}

		auto drawer = child[0].to_object<Widgets::Drawer>();

		MX::Rectangle rect(0.0f, 0.0f, -1.0f, -1.0f);

		if ( child.size() > 1 )
			PropertyLoader<MX::Rectangle>::load( rect, child.array()[1] );

		AddChild(drawer, rect);
	}

}

void CompositeLayouterDrawer::AddChild(const std::shared_ptr<Widgets::Drawer> &drawer, const MX::Rectangle &rectangle)
{
	_children.emplace_back(drawer, rectangle);
}

void CompositeLayouterDrawer::DrawBackground()
{
	for (auto &child : _children)
		child.DrawBackground();
}




class ButtonStateDrawer : public CompositeDrawer
{
public:
	struct State
	{
		void Set(const std::string& child, State* defaultState = nullptr)
		{
			if (defaultState)
			{
				_drawer = defaultState->_drawer;
				_qimage = defaultState->_qimage;
				_image = defaultState->_image;
				_color = defaultState->_color;
			}

			ScriptObjectString script(child);

			script.load_property(_drawer, "Drawer");

			if (!script.load_property(_qimage, "Image"))
				script.load_property(_image, "Image");

			script.load_property(_color, "Color");
		}


		operator bool()
		{
			return hasDrawer() || hasImage();
		}

		bool hasImage()
		{
			return _image || _qimage;
		}

		bool hasDrawer()
		{
			return _drawer != nullptr;
		}

		void DrawImage(const MX::Color& color, int pos)
		{
			if (_qimage)
				_qimage->DrawArea(Destination::current().rectangle, MX::Widgets::Widget::current().geometry.color * _color);
			else if (_image)
			{
                auto r = MX::Rectangle::fromWH(.0f, .0f, _image->Width(), _image->Height());
                r.NumLayoutIn(Destination::current().rectangle, pos);
				_image->DrawTinted( { r.x1, r.y1 }, MX::Widgets::Widget::current().geometry.color * _color );
			}
		}

		void DrawBackground()
		{
			if (_drawer)
				_drawer->DrawBackground();
		}
	protected:
		std::shared_ptr<Widgets::Drawer> _drawer;
		std::shared_ptr<Graphic::QuadImage> _qimage;
		std::shared_ptr<Graphic::Image> _image;
		MX::Color _color;

	};


	ButtonStateDrawer(LScriptObject& script) : CompositeDrawer(script)
	{
		_default.Set(script.pathToChild("Default"));

		_normal.Set(script.pathToChild("Normal"), &_default);
		_hover.Set(script.pathToChild("Hover"), &_default);
		_pressed.Set(script.pathToChild("Pressed"), &_default);
		_disabled.Set(script.pathToChild("Disabled"), &_default);

        script.load_property(_position, "Pos");
	}


	void DrawBackground()
	{
		auto &button = dynamic_cast<ButtonWidget&>(Widget::current());

		State* state = &_normal;

		if (!button.enabled())
			state = &_disabled;
		else if (button.pressed() || button.selected())
			state = &_pressed;
		else if (_hover && button.hover())
			state = &_hover;


		if (!button.enabled() && !state->hasImage() && !state->hasDrawer())
		{
			MX::Color color = MX::Widgets::Widget::current().geometry.color;
			color.Set(color.r() / 2.0f, color.g() / 2.0f, color.b() / 2.0f);
			color.SetA(color.a() / 2.0f);
			_normal.DrawImage(color, _position);
		}
		else
		{
			state->DrawImage(MX::Widgets::Widget::current().geometry.color, _position);
			state->DrawBackground();
		}

	}



protected:
    int _position = 5;
	State _default;

	State _normal;
	State _hover;
	State _pressed;
	State _disabled;

};

class IfButtonStateDrawer : public Widgets::Drawer
{
public:

	IfButtonStateDrawer(LScriptObject& script)
	{
		script.load_property_children(_drawers, "Drawers");
		script.load_property(_drawer, "Drawer");
		script.load_property(_elseDrawer, "ElseDrawer");


		std::string state;
		if (!script.load_property(state, "State"))
		{
			if (script.load_property(state, "NotState"))
			{
				std::swap(_drawer, _elseDrawer);
			}
		}

		static std::map<std::string, Widgets::ButtonWidget::State> strToState = {
			{ "Enabled", Widgets::ButtonWidget::State::Enabled },
			{ "Hover", Widgets::ButtonWidget::State::Hover },
			{ "Pressed", Widgets::ButtonWidget::State::Pressed },
			{ "Selected", Widgets::ButtonWidget::State::Selected }
		};

		auto it = strToState.find(state);
		if (it != strToState.end())
			_state = it->second;
	}


	void DrawBackground()
	{
        auto button = dynamic_cast<ButtonWidget*>(&(Widget::current()));
        if (button && button->isState(_state))
        {
            if (_drawer)
                _drawer->DrawBackground();

            for (auto &drawer : _drawers)
                drawer->DrawBackground();
        }
        else if (_elseDrawer)
            _elseDrawer->DrawBackground();
	}



protected:
	std::vector<std::shared_ptr<Widgets::Drawer>> _drawers;
	std::shared_ptr<Widgets::Drawer> _drawer;
	std::shared_ptr<Widgets::Drawer> _elseDrawer;

	Widgets::ButtonWidget::State _state = Widgets::ButtonWidget::State::Enabled;
};


class SwitchStateDrawer : public Widgets::Drawer
{
public:
	SwitchStateDrawer(LScriptObject& script)
	{
		script.load_property(_condition, "Condition");
		script.load_property_children(_drawers, "Case");
		script.load_property(_defaultCase, "Default");
	}

	void DrawBackground()
	{
		float cond = *_condition;

		auto it = _drawers.find(cond);
		if (it == _drawers.end())
		{
			if (_defaultCase)
				_defaultCase->DrawBackground();
			return;
		}
			

		it->second->DrawBackground();
	}

protected:
	Scriptable::Value::pointer _condition;
	std::map<float, std::shared_ptr<Widgets::Drawer>> _drawers;
	std::shared_ptr<Widgets::Drawer>                  _defaultCase;
};


class IfDrawer : public Widgets::Drawer
{
public:
	IfDrawer(LScriptObject& script)
	{
		script.load_property(_condition, "Condition");
		script.load_property_children(_drawers, "Drawers");
		script.load_property(_elseDrawers, "Else");

		assert(_condition);
	}


	void DrawBackground()
	{
		if (!_condition)
			return;

		float cond = *_condition;

		auto& drawers = cond == 0.0f ? _elseDrawers : _drawers;

		for (auto &drawer : drawers)
			drawer->DrawBackground();
	}

protected:

	Scriptable::Value::pointer _condition;
	std::vector<std::shared_ptr<Widgets::Drawer>>  _drawers;
	std::vector<std::shared_ptr<Widgets::Drawer>>  _elseDrawers;
};


class BlenderDrawer : public Widgets::Drawer
{
public:
	BlenderDrawer(LScriptObject& script)
	{
		script.load_property_children(_drawers, "Drawers");
		script.load_property(_blender, "Blender");
	}


	void DrawBackground()
	{
		auto g = _blender.Use();
		for (auto &drawer : _drawers)
			drawer->DrawBackground();
	}

protected:
	MX::Graphic::Blender                           _blender{ GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
	std::vector<std::shared_ptr<Widgets::Drawer>>  _drawers;
};


class CurrentDrawer : public Widgets::Drawer
{
public:
    CurrentDrawer(LScriptObject& script){ }

    void DrawBackground()
    {
        if (!Context<Widgets::Drawer>::isCurrent())
            return;
        Context<Widgets::Drawer>::current().DrawBackground();
    }
};

void MX::Widgets::CompositeDrawersInit::Init()
{
	ScriptClassParser::AddCreator(L"Drawer.Composite.Blender", new OutsideScriptClassCreatorContructor<BlenderDrawer>());

	ScriptClassParser::AddCreator(L"Drawer.Composite.Layouter", new OutsideScriptClassCreatorContructor<CompositeLayouterDrawer>());
	ScriptClassParser::AddCreator(L"Drawer.Composite.ButtonState", new OutsideScriptClassCreatorContructor<ButtonStateDrawer>());
	ScriptClassParser::AddCreator(L"Drawer.Composite.IfButtonState", new OutsideScriptClassCreatorContructor<IfButtonStateDrawer>());

	ScriptClassParser::AddCreator(L"Drawer.Composite.Switch", new OutsideScriptClassCreatorContructor<SwitchStateDrawer>());

	ScriptClassParser::AddCreator(L"Drawer.Composite.If", new OutsideScriptClassCreatorContructor<IfDrawer>());
    ScriptClassParser::AddCreator(L"Drawer.Composite.Current", new OutsideScriptClassCreatorContructor<CurrentDrawer>());
}






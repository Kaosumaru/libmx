#include "WidgetFactory.h"
#include "Script/ScriptClassParser.h"
#include "Scene/Script/Event.h"
#include "Widgets/Label.h"


using namespace MX;
using namespace MX::Widgets;


WidgetFactory::WidgetFactory(LScriptObject& script)
{
	script.load_property(_drawer, "Drawer");
}


WidgetPtr WidgetFactory::Create()
{
	auto widget = WidgetCreate();
	if (_drawer)
		widget->SetDrawer(_drawer);
	return widget;
}

class LabelWidgetFactory : public WidgetFactory
{
public:
	LabelWidgetFactory(LScriptObject& script) : WidgetFactory(script)
	{
		script.load_property(_text, "Text");
	}

#ifdef WIPFONT
	WidgetPtr WidgetCreate() override
	{ 
		return MX::make_shared<MX::Widgets::Label>(_text ? _text->text() : L"");
	}
#endif
protected:
	Scriptable::Value::pointer _text;
};

#ifdef WIPFONT
class LabelButtonWidgetFactory : public LabelWidgetFactory
{
public:
	LabelButtonWidgetFactory(LScriptObject& script) : LabelWidgetFactory(script)
	{
		_data = std::make_shared<Data>();
		script.load_property(_data->_onClicked, "OnClicked");
	}


	WidgetPtr WidgetCreate() override
	{
		auto button = MX::make_shared<MX::Widgets::LabelButton>(_text ? _text->text() : L"");

		button->onClicked.connect([=, b = button.get()]() 
		{
			auto guard = Context<Widget>::Lock(b);
			_data->_onClicked.Do();
		});

		return button;
	}
protected:
	struct Data
	{
		MX::EventHolder   _onClicked;
	};
	
	std::shared_ptr<Data> _data;
};
#endif

void WidgetFactoryInit::Init()
{
	ScriptClassParser::AddCreator(L"Widget.Widget", new OutsideScriptClassCreatorContructor<WidgetFactory>());
#ifdef WIPFONT
	ScriptClassParser::AddCreator(L"Widget.Label", new OutsideScriptClassCreatorContructor<LabelWidgetFactory>());
	ScriptClassParser::AddCreator(L"Widget.LabelButton", new OutsideScriptClassCreatorContructor<LabelButtonWidgetFactory>());
#endif
}

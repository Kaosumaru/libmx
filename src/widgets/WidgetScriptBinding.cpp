#include "WidgetScriptBinding.h"
#include "script/Script.h"
#include "widgets/Drawers/Drawer.h"
#include "widgets/Widget.h"
#include "widgets/Strategies/DrawableStrategy.h"
#include "RPN/Function.h"


using namespace MX;
using namespace MX::Widgets;



void WidgetScriptBinding::Init()
{
	Script::onParsed.static_connect([&]() { onReload(); });
	onReload();
}


void WidgetScriptBinding::onReload()
{
	auto &script = Script::current();
	script.SetPairFunctor("Area.X", []()-> float { return Drawer::Destination::current().x(); });
	script.SetPairFunctor("Area.Y", []()-> float { return Drawer::Destination::current().y(); });
	script.SetPairFunctor("Area.Width", []()-> float { return Drawer::Destination::current().width(); });
	script.SetPairFunctor("Area.Height", []()-> float { return Drawer::Destination::current().height(); });
	script.SetPairFunctor("Area.AspectRatio", []()-> float { if (!Drawer::Destination::isCurrent()) return 0.0f; return Drawer::Destination::current().width() / Drawer::Destination::current().height(); });

	script.SetPairFunctor("Button.Pressed", []()-> float { return dynamic_cast<ButtonWidget&>(Widget::current()).pressed() ? 1.0f : 0.0f; });
	script.SetPairFunctor("Button.Selected", []()-> float { return dynamic_cast<ButtonWidget&>(Widget::current()).selected() ? 1.0f : 0.0f; });
	script.SetPairFunctor("Button.Enabled", []()-> float { return dynamic_cast<ButtonWidget&>(Widget::current()).enabled() ? 1.0f : 0.0f; });
	script.SetPairFunctor("Button.Hover", []()-> float { return dynamic_cast<ButtonWidget&>(Widget::current()).hover() ? 1.0f : 0.0f; });

	script.SetPairFunctor("Widget.Progress", []()-> float 
		{ 
			if (!ProgressData::isCurrent())
			{
				assert(false);
				return 0.0f;
			}
			return ProgressData::current().value(); 
		});


	RPN::Functions::AddLambda("Widget.Property", [](const std::string& name) { return Widget::current().properties().valueOf<float>(name); });

	script.SetPairFunctor("Widget.Text.Width", []()-> float 
	{ 
		return ScopeSingleton<TextData>::current().actualWidth();
	});
}
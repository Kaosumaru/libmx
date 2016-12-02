#include "Label.h"
#include "application/Window.h"
#include "Game/Resources/Resources.h"
#include <iostream>

using namespace MX;
using namespace MX::Widgets;

#ifdef WIPFONT
#include "HTML/HTMLRendererCairo.h"

MX::Widgets::Label::Label(const std::wstring &text)
{
	AddStrategy(_textStrategy);
	_margins = MX::Margins(15,15,10,10);
	SetText(text);
}

MX::Widgets::Label::Label()
{
	AddStrategy(_textStrategy);
	_margins = MX::Margins(15, 15, 10, 10);
}



AutoLabel::AutoLabel()
{
	_textStrategy = std::make_shared<Strategy::Drawable::AutoText>();
	AddStrategy(_textStrategy);
}




LabelButton::LabelButton(const std::wstring &text) : Label(text)
{
	AddStrategy(_buttonStrategy);
}

LabelButton::LabelButton() : Label()
{
	AddStrategy(_buttonStrategy);
}



FPSLabel::FPSLabel() : Label()
{

}

float FPSLabel::fps()
{
	if (count == 0)
		return 0.0f;
	auto averange = sum / (float)count;
	return 1.0f / averange;
}

void FPSLabel::Run()
{
	_timer.Step();
	sum += _timer.elapsed_seconds();
	count++;
	Label::Run();
	if (_stopWatch.Tick())
	{
		_stopWatch.Start(defaultTime());
		std::wstringstream ss;
		ss.precision(4);
		ss << fps() << " fps";
		SetText(ss.str());

		sum = 0.0f;
		count = 0;
	}
}
#endif
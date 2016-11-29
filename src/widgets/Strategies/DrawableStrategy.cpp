#include "DrawableStrategy.h"
#include "../Widget.h"
#include "Application/Window.h"
#include "Game/Resources/Resources.h"
#include "Utils/ContextStack.h"
#include "HTML/HTMLRendererCairo.h"
#include "Graphic/Fonts/Font.h"
#include <iostream>

using namespace MX;
using namespace MX::Widgets;
using namespace MX::Strategies;
using namespace MX::Strategies::Drawable;


void MX::Widgets::TextData::UpdateTextImage()
{
	if (!_dirty)
		return;
	Graphic::Font::pointer font = _font ? _font : MX::Graphic::Font::CreateDefault();


	if (_html)
	{
		SetTextImage(HTMLRendererCairo::DrawOnBitmap(_text, _width, _font));
	}
	else
	{
		SetTextImage(font->DrawTextOnBitmap(_text, _width));
	}
}


void Image::BeforeDraw()
{
	_old = &(ScopeSingleton<Graphic::Image>::current());
	ScopeSingleton<Graphic::Image>::SetCurrent(*_image);
}
void Image::AfterDraw()
{
	ScopeSingleton<Graphic::Image>::SetCurrent(*_old);
	_old = nullptr;
}


void Text::BeforeDraw()
{ 
	_old = &(ScopeSingleton<TextData>::current());
	ScopeSingleton<TextData>::SetCurrent(_textData);

	ContextStack<TextData>::PushCurrent(&_textData);
}

void Text::AfterDraw() 
{
	ContextStack<TextData>::PopCurrent(&_textData);

	ScopeSingleton<TextData>::SetCurrent(*_old);
	_old = nullptr;
}


void Progress::BeforeDraw()
{
	_old = &(ScopeSingleton<ProgressData>::current());
	ScopeSingleton<ProgressData>::SetCurrent(_progressData);

}

void Progress::AfterDraw()
{
	ScopeSingleton<ProgressData>::SetCurrent(*_old);
	_old = nullptr;
}



ProgressFromSignalizing::ProgressFromSignalizing()
{
	_data = std::make_shared<int>();
}

ProgressFromSignalizing::ProgressFromSignalizing(SignalizingVariable<float>& value, SignalizingVariable<float>& max)
{
	_data = std::make_shared<int>();
	SetVariables(&value, &max);
}

void ProgressFromSignalizing::SetVariables(SignalizingVariable<float>* value, SignalizingVariable<float>* max)
{
	_value = value;
	_max = max;
	if (_value)
		_value->onValueChanged.connect(std::bind(&ProgressFromSignalizing::OnValueChanged, this), _data);
	if (_max)
		_max->onValueChanged.connect(std::bind(&ProgressFromSignalizing::OnValueChanged, this), _data);
	OnValueChanged();
}

void ProgressFromSignalizing::OnValueChanged()
{
	progress().Set((_value && _max && *_max > 0.0f && *_value > 0.0f) ? *_value / *_max : 0.0f);
}
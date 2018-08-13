#include "DrawableStrategy.h"
#include "../Widget.h"
#include "application/Window.h"
#include "game/resources/Resources.h"
#include "utils/ContextStack.h"
#include "graphic/fonts/Font.h"
#include "graphic/images/TextureImage.h"
#include "html/HTMLRendererFreetype.h"
#include "html/HTMLRendererQueue.h"

#include <iostream>

using namespace MX;
using namespace MX::Widgets;
using namespace MX::Strategies;
using namespace MX::Strategies::Drawable;


void MX::Widgets::TextData::UpdateTextImage()
{
	if (!_dirty)
		return;

	if (!_bitmapFont)
	{
		Graphic::Font::pointer font = _font ? _font : MX::Graphic::Font::CreateDefault();

		if (_html)
		{
			SetTextImage(HTMLRendererFreetype::DrawOnBitmap(_text, _width, _font));
		}
		else
		{
			SetTextImage(font->DrawTextOnBitmap(_text));
		}
	}
	else
	{
		_renderQueue.Clear();
		auto bitmapFont = _bitmapFont->parentFont();
		if (_html)
		{
			MX::HTMLRendererQueue::Options options;
			options.default_size = _bitmapFont->size();
			_renderQueue = MX::HTMLRendererQueue::Render(_text.c_str(), _width, bitmapFont, options);
		}
		else
		{
			auto scale = bitmapFont->scaleForSize(_bitmapFont->size());
			bitmapFont->QueueText(_renderQueue, _text.c_str(), {}, scale);
		}
		_dirty = false;
	}
}

unsigned MX::Widgets::TextData::actualWidth()
{
	if (isBitmapFont()) return _renderQueue.bounds().x;
	auto tex = textImage();
	if (tex)
		return tex->Width();
	return 0;
}

unsigned MX::Widgets::TextData::actualHeight()
{
	if (isBitmapFont()) return _renderQueue.bounds().y;
	auto tex = textImage();
	if (tex)
		return tex->Height();
	return 0;
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
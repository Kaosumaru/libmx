#pragma once
#include <memory>
#include <string>
#include "graphic/fonts/BitmapFont.h"

namespace MX
{

	class HTMLRendererQueue
	{
	public:
		static Graphic::RenderQueue Render(const char* str, float width, const std::shared_ptr<Graphic::BitmapFont>& defaultFont = nullptr);
	};

}


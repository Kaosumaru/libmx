#pragma once
#include <memory>
#include <string>
#include "graphic/fonts/BitmapFont.h"

namespace MX
{

	class HTMLRendererQueue
	{
	public:
		struct Options
		{
			float default_size = 0.0f;
		};
		static Graphic::RenderQueue Render(const char* str, float width, const std::shared_ptr<Graphic::BitmapFont>& defaultFont = nullptr, const Options& options = {});
		static Graphic::RenderQueue Render(const wchar_t* str, float width, const std::shared_ptr<Graphic::BitmapFont>& defaultFont = nullptr, const Options& options = {});
	};

}


#pragma once
#include "TextureRenderer.h"
#include <string>

namespace MX{

	namespace gl
	{
		class Program;
	}


	namespace Graphic
	{
		class Renderers : public DeinitSingleton<Renderers>
		{
		public:
			Renderers();

			auto& textRenderer() { return *_textRenderer; }
			auto& defaultRenderer() { return *_defaultRenderer; }

			std::shared_ptr<gl::Program> createProgram(const char* fragmentPath, const char* vertexPath = nullptr);
			const char* defaultVertexShaderPath() { return "shader/instanced.vertex"; }
		protected:
			std::shared_ptr<MX::Graphic::TextureRenderer> _defaultRenderer;
			std::shared_ptr<MX::Graphic::TextureRenderer> _textRenderer;
		};
	}
}

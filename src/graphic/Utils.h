#pragma once
#include<memory>
#include<vector>
#include "cinder/gl/GlslProg.h"
#include "graphic/Shader/Shader.h"
#include "graphic/Renderers/InstancedGLRenderer.h"
#include "graphic/images/Surface.h"
#include "utils/Time.h"

namespace cinder {

	class Shape2d;
};

namespace MX
{
	namespace Scriptable
	{
		class Variable;
	}

	inline float exponential(float value) { return sin(value * 3.14f / 2); }
	inline float inverse_exponential(float value) { return sin((1.0f - value) * 3.14f / 2); }

	namespace Graphic
	{
		struct Utils
		{
			struct ShaderData
			{
				ci::gl::GlslProgRef shader;

				using UniformFunction = std::function<std::shared_ptr<UniformBase>()>;
				std::vector<UniformFunction> _uniformCreators;
			};

			static ShaderData prepareShader(const std::string& vertex_path, const std::string& fragment_path, const std::map<std::string, std::string>& defines = {});


			static std::string ShapeToString(const cinder::Shape2d &path);
			static cinder::Shape2d StringToShape(const std::string &path);

			static void DrawPolygon(const cinder::Shape2d &path, const MX::Color &color = MX::Color());
			static void DrawPolygon(const cinder::Path2d &path, const MX::Color &color = MX::Color());
		};


		class ShaderContainer
		{
		protected:
			ShaderContainer(LScriptObject& script);

			void EnableSamplers()
			{
				int i = 0;
				for (auto& sampler : _samplers)
					sampler->native_bitmap_handle()->bind(++i);

			}

			void DisableSamplers()
			{
				int i = 0;
				for (auto& sampler : _samplers)
					sampler->native_bitmap_handle()->unbind(++i);
			}

			void ApplyUniforms()
			{
				for (auto &uniform : _uniforms)
					uniform->ApplyRaw();
			}


			std::vector<std::shared_ptr<MX::Graphic::UniformBase>> _uniforms;
			std::shared_ptr<MX::Graphic::ScriptableTimedProgram> _shader;
			std::vector<Graphic::Surface::pointer> _samplers;
		};

		class ShaderContainerRenderer : public ShaderContainer
		{
		protected:
			ShaderContainerRenderer(LScriptObject& script) : ShaderContainer(script)
			{

				if (_shader && _shader->shader())
					_renderer = std::make_shared<MX::Graphic::InstanceGLRenderer>(_shader);
			}


			void DrawUsingShader()
			{
				if (!_renderer)
				{
					OnDrawUsingShader();
					return;
				}

				if (_shader)
					_shader->time = Time::Timer::current().total_seconds();

				auto lock = _renderer->Use();
				_shader->bind();

				ApplyUniforms();

				EnableSamplers();
				OnDrawUsingShader();
				Graphic::TextureRenderer::current().Flush();
				DisableSamplers();
				_shader->unbind();
			}

			virtual void OnDrawUsingShader()
			{

			}

			std::shared_ptr<MX::Graphic::InstanceGLRenderer> _renderer;
		};

	}


}


void operator & (cinder::Shape2d &path, MX::Scriptable::Variable&& var);

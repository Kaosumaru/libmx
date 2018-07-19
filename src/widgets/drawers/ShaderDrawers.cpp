#include "ShaderDrawers.h"
#include "CompositeDrawers.h"
#include "widgets/Widget.h"
#include "script/ScriptClassParser.h"
#include "utils/shapes/Rectangle.h"
#include "graphic/opengl/ProgramInstance.h"
#include "graphic/renderer/InstancedRenderer.h"
#include "graphic/renderer/DefaultRenderers.h"

#include <iostream>

using namespace MX;
using namespace MX::Widgets;




class CompositeLayouterDrawerShader : public CompositeLayouterDrawer
{
public:
	CompositeLayouterDrawerShader(LScriptObject& script) : CompositeLayouterDrawer(script)
	{
		// TODO refactor
		auto createProgram = [](const char* vertexPath, const char* fragmentPath)
		{
			std::string out;
			auto program = gl::createProgramFromFiles(vertexPath, fragmentPath, out);
			if (program)
			{
				std::cout << "Shader compiled" << std::endl;
			}
			else
			{
				std::cout << "Shader error: " << out << std::endl;
			}
			return program;
		};

		std::string vertexPath, fragmentPath;
		if (!script.load_property(vertexPath, "VertexPath"))
			vertexPath = Graphic::Renderers::get().defaultVertexShaderPath();
		script.load_property(fragmentPath, "FragmentPath");


		_instance = std::make_shared<gl::ProgramInstance>(createProgram(vertexPath.c_str(), fragmentPath.c_str()));
		if (_instance)
			_renderer = std::make_shared<Graphic::InstancedRenderer>(_instance);
	}

	void DrawBackground() override
	{
		if (!_renderer) return;
		MX::Graphic::TextureRenderer::Context guard(*_renderer);
		CompositeLayouterDrawer::DrawBackground();
	}
protected:
	std::shared_ptr<Graphic::TextureRenderer> _renderer;
	std::shared_ptr<gl::ProgramInstance> _instance;
};
//MXREGISTER_CLASS(L"Drawer.Composite.Layouter.Shader", CompositeLayouterDrawerShader)

void MX::Widgets::ShaderDrawersInit::Init()
{
	ScriptClassParser::AddCreator(L"Drawer.Composite.Layouter.Shader", new OutsideScriptClassCreatorContructor<CompositeLayouterDrawerShader>());
}

/*
template<typename Parent>
class ShaderizeDrawer : public Parent, public Graphic::ShaderContainerRenderer
{
public:
	ShaderizeDrawer(LScriptObject& script) : Parent(script), ShaderContainerRenderer(script)
	{

	}

	void DrawBackground() override
	{
		DrawUsingShader();
	}

protected:
	void OnDrawUsingShader() override
	{
		Parent::DrawBackground();
	}
};
*/




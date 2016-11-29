#include "MXShaderDrawers.h"
#include "Utils/MXUtils.h"
#include "MXCompositeDrawers.h"
#include "Graphic/Images/MXImage.h"
#include "Graphic/Images/MXSlice9Image.h"

#include "Widgets/MXWidget.h"
#include "Script/MXScriptClassParser.h"
#include "Script/Class/MXScriptImageClass.h"
#include "Game/Resources/MXResources.h"
#include "Widgets/MXLabel.h"
#include "Graphic/Images/MXSlice9Image.h"

#include "Graphic/Renderers/MXInstancedGLRenderer.h"
#include "Graphic/Shader/MXShader.h"

#include "MXDrawers.h"

#include "Graphic/MXUtils.h"
#include "Graphic/Shader/MXShader.h"


#include "Utils/MXDebugGUIManager.h"
using namespace MX;







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



template<typename T>
class UniformExpr : public Graphic::UniformBase
{
public:
	UniformExpr(LScriptObject& script) : _program(Graphic::ProgramBase::current())
	{
		assert(_program);
		std::string name;

		bool valid = true;
		valid &= script.load_property(name, "n");
		valid &= script.load_property(_value, "v");
		assert(valid);
		if (valid)
		{
			_location = _program->shader()->getUniformLocation(name);
			AddSelfToShader(_program);
		}
	}

protected:
	void ApplyRaw() override
	{
		assert(Graphic::ProgramBase::current() == _program);
		Graphic::UniformInfo<T>::Set(_location, *_value);
	}

	void Apply() override
	{
		assert(Graphic::ProgramBase::current() == _program);
		_program->bind();
		Graphic::UniformInfo<T>::Set(_location, *_value);
		_program->unbind();
	}

	
	GLuint _location;
	Scriptable::Value::pointer _value;
	Graphic::ProgramBase *_program;
};


template<typename T>
class ScriptableUniform : public MX::Graphic::Uniform<T>
{
public:
	ScriptableUniform(LScriptObject& script) : Uniform<T>(MX::Graphic::ProgramBase::current())
	{
		assert(_program);
		std::string name;
		T value;

		bool valid = true;
		valid &= script.load_property(name, "n");
		valid &= script.load_property(value, "v");
		assert(valid);
		if (valid)
		{
			_location = _program->shader()->getUniformLocation(name);
			Set(value);
			AddSelfToShader();
		}
	}
};


class DebugShader
{
public:
	DebugShader(LScriptObject& script, MX::Graphic::ScriptableProgram& program)
	{
		script.load_property(_samplers, "Samplers");


		{
			float debug = false;
			script.load_property(debug, "Debug");
			if (!debug)
				return;
		}

		program.bind();
		script.load_property(_uniforms, "Debug.Uniforms");
		program.unbind();

		using namespace ci;

		std::string name;
		script.load_property(name, "Name");

		static int ix = 0;
		++ix;
		std::stringstream ss;
		ss << name << "-" << ix;

		_params = params::InterfaceGl::create(ss.str(), app::toPixels(ivec2(200, 400)));

		ShowUniforms(script, program);

		_params->minimize();
#ifndef MX_GAME_RELEASE
		MX::DebugGUIManager::get().AddInterface(_params);
#endif
	}

	void ShowUniforms(LScriptObject& script, MX::Graphic::ScriptableProgram& program)
	{
		std::vector<Scriptable::Value::pointer> uniforms_p;
		script.load_property(uniforms_p, "Debug.Uniforms");

		for (decltype(uniforms_p)::size_type i = 0; i < _uniforms.size(); i ++)
		{
			ScriptObjectString script{ uniforms_p[i]->fullPath() };
			auto &uniform = _uniforms[i];
			GenerateForUniform(_params, uniform, script);
		}
	}

	static const std::map<std::string, std::string>& additionalDefines(LScriptObject& script)
	{
		float debug = false;
		script.load_property(debug, "Debug");

		static std::map<std::string, std::string> mnone = {};
		static std::map<std::string, std::string> mdebug = { { "VDEBUG", "1" } };
		return debug ? mdebug : mnone;

	}

	using UniformPointer = std::shared_ptr<MX::Graphic::UniformBase>;

	template<typename T>
	struct UniformGenerator
	{
		void operator()(ci::params::InterfaceGlRef params, UniformPointer& uniform, LScriptObject& script)
		{
			using UniformType = MX::Graphic::Uniform<T>;

			auto u = std::dynamic_pointer_cast<UniformType>(uniform);
			if (!u)
				return;

			std::string name;
			script.load_property(name, "n");

			std::function<void(T)> setter = [=](T a) { *u = a; };
			std::function<T()> getter = [=]() { return u->value(); };
			auto options = params->addParam(name, setter, getter);

			float varf;
			if (script.load_property(varf, "step"))
				options.step(varf);

			if (script.load_property(varf, "min"))
				options.min(varf);

			if (script.load_property(varf, "max"))
				options.max(varf);


			std::string vars;
			if (script.load_property(vars, "group"))
				options.group(vars);
		};
	};

	
	static void GenerateForUniform(const ci::params::InterfaceGlRef &params, UniformPointer& uniform, LScriptObject& script)
	{

		
		using Function = std::function<void(ci::params::InterfaceGlRef params, UniformPointer& uniform, LScriptObject& script)>;
		static std::map<std::type_index, Function> functions;


		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;

			functions[typeid(ScriptableUniform<float>)] = UniformGenerator<float>{};
			functions[typeid(ScriptableUniform<int>)] = UniformGenerator<int>{};

			functions[typeid(ScriptableUniform<ci::ColorA>)] = UniformGenerator<ci::ColorA>{};
			functions[typeid(ScriptableUniform<ci::Color>)] = UniformGenerator<ci::Color>{};

			//functions[typeid(ScriptableUniform<MX::Vector2>)] = UniformGenerator<MX::Vector2>{};
			//functions[typeid(ScriptableUniform<ci::vec2>)] = UniformGenerator<ci::vec2>{};
			functions[typeid(ScriptableUniform<ci::vec3>)] = UniformGenerator<ci::vec3>{};
		}

		auto &f = functions[typeid(*uniform)];
		if (f)
			f(params, uniform, script);
	}

	~DebugShader()
	{
#ifndef MX_GAME_RELEASE
		if (_params)
			MX::DebugGUIManager::get().RemoveInterface(_params);
#endif
	}

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


protected:
	ci::params::InterfaceGlRef _params;


	std::vector<std::shared_ptr<MX::Graphic::UniformBase>> _uniforms;
	std::shared_ptr<MX::Graphic::ScriptableTimedProgram> _shader;
	std::vector<Graphic::Surface::pointer> _samplers;
};





class ScriptableProgramUniforms : public MX::Graphic::ScriptableProgram, public DebugShader
{
public:
	ScriptableProgramUniforms(LScriptObject& script) : ScriptableProgram(script, additionalDefines(script)), DebugShader(script, *this)
	{

	}

	void onBound() override
	{
		ApplyStaticUniforms();
		DebugShader::ApplyUniforms();
		EnableSamplers();
	}
	void onUnbound() override
	{
		DisableSamplers();
	}

};

class ScriptableTimedProgramUniforms : public MX::Graphic::ScriptableTimedProgram, public DebugShader
{
public:
	ScriptableTimedProgramUniforms(LScriptObject& script) : ScriptableTimedProgram(script, additionalDefines(script)),  DebugShader(script, *this)
	{

	}

	void onBound() override 
	{
		ApplyStaticUniforms();
		DebugShader::ApplyUniforms();
		EnableSamplers();
	}
	void onUnbound() override
	{
		DisableSamplers();
	}
};




void MX::Widgets::ShaderDrawersInit::Init()
{
	ScriptClassParser::AddCreator(L"Drawer.Color.Shader", new OutsideScriptClassCreatorContructor<ShaderizeDrawer<ColorDrawer>>());
	ScriptClassParser::AddCreator(L"Drawer.Composite.Layouter.Shader", new OutsideScriptClassCreatorContructor<ShaderizeDrawer<CompositeLayouterDrawer>>());
	ScriptClassParser::AddCreator(L"Drawer.Image.Shader", new OutsideScriptClassCreatorContructor<ShaderizeDrawer<ImageDrawer>>());



	ScriptClassParser::AddCreator(L"Shader.Timed", new DefaultCachedClassCreatorContructor<MX::Graphic::ScriptableTimedProgram, OutsideScriptClassCreatorContructor_Policy>([](auto &prog) { return prog; }));

	ScriptClassParser::AddCreator(L"Shader.Uniform.Timed", new DefaultCachedClassCreatorContructor<ScriptableTimedProgramUniforms, OutsideScriptClassCreatorContructor_Policy>([](auto &prog) { return prog; }));
	ScriptClassParser::AddCreator(L"Shader.Uniform", new DefaultCachedClassCreatorContructor<ScriptableProgramUniforms, OutsideScriptClassCreatorContructor_Policy>([](auto &prog) { return prog; }));


	ScriptClassParser::AddCreator(L"GL.Uniform.Int", new OutsideScriptClassCreatorContructor<ScriptableUniform<int>>());
	ScriptClassParser::AddCreator(L"GL.Uniform.Float", new OutsideScriptClassCreatorContructor<ScriptableUniform<float>>());
	ScriptClassParser::AddCreator(L"GL.Uniform.Vec2", new OutsideScriptClassCreatorContructor<ScriptableUniform<Vector2>>());
	ScriptClassParser::AddCreator(L"GL.Uniform.Vec3", new OutsideScriptClassCreatorContructor<ScriptableUniform<ci::vec3>>());

	ScriptClassParser::AddCreator(L"GL.Uniform.Int.Expr", new OutsideScriptClassCreatorContructor<UniformExpr<int>>());
	ScriptClassParser::AddCreator(L"GL.Uniform.Float.Expr", new OutsideScriptClassCreatorContructor<UniformExpr<float>>());
}
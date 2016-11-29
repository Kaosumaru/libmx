#include "MXUtils.h"
#include "Game/Resources/MXPaths.h"
#include "boost/wave.hpp"
#include "boost/wave/cpplexer/cpp_lex_iterator.hpp"
#include <iterator>
#include <sstream>

#include "cinder/gl/gl.h"
#include "Graphic/Renderers/MXCinderRenderer.h"


#include "Script/Scriptable/MXScriptableVariable.h"
#include "Script/MXPropertyLoaders.h"

#include "Script/MXScriptParser.h"

using namespace MX;
using namespace Graphic;

namespace MX
{
namespace detail
{
	struct my_hooks : public boost::wave::context_policies::default_preprocessing_hooks
	{
		
		template <typename ContextT, typename ContainerT>
		bool
			emit_line_directive(ContextT const& ctx, ContainerT &pending,
				typename ContextT::token_type const& act_token)
		{
			//don't output any lines
			//there seems to be an bug (if pending was empty, next token was duplicated)
			//TODO check this and report

			//so instead I'm commenting out this line
			typename ContextT::position_type pos = act_token.get_position();
			using result_type = typename ContextT::token_type;
			pending.push_back(result_type(boost::wave::T_CCOMMENT, "//", pos));

			return false;
		}

		//ignore unrecognized extensions
		template <typename ContextT, typename ContainerT>
		bool
			found_unknown_directive(ContextT const& ctx, ContainerT const& line,
				ContainerT& pending)
		{
			for (auto& token : line)
				pending.push_back(token);
			return true;
		}   // by default we never interpret unknown directives
	};


	struct pp_context
	{
		using lex_iterator_type = boost::wave::cpplexer::lex_iterator< boost::wave::cpplexer::lex_token<> >;
		using context_type = boost::wave::context< std::string::iterator, lex_iterator_type, boost::wave::iteration_context_policies::load_file_to_string, my_hooks >;
		using iterator_type = context_type::iterator_type;

		iterator_type &first;
		iterator_type &last;

		std::stringstream &ss;

	};

	void process(pp_context &ctx)
	{
		auto first = ctx.first;
		auto last = ctx.last;


		std::string object_to_generate;
		std::string object_name;

		

		//auto &value = first->get_value();
		auto token_id = [&]() { return first->operator boost::wave::token_id(); };

		auto eat_spaces = [&]()
		{
			while (first != last && token_id() == boost::wave::T_SPACE)
				first++;
		};

		//add pragma bind
		if (token_id() != boost::wave::T_PP_PRAGMA)
			return;
		++first;
		eat_spaces();
		if (first == last)
			return;
		if (first->get_value() != "mx_bind")
			return;
		++first;
		eat_spaces();

		{
			std::stringstream ss;
			while (token_id() != boost::wave::T_NEWLINE && first != last)
				ss << first->get_value(), first++;

			object_to_generate = ss.str();
		}
		auto cut_to_here = first;


		first++;
		eat_spaces();
		if (first == last)
			return;
		if (first->get_value() != "uniform")
			return;

		first++;
		eat_spaces();
		auto id = token_id();
		//if (token_id() != boost::wave::T_IDENTIFIER)
		//	return;

		first++;
		eat_spaces();

		{
			std::stringstream ss;
			while (token_id() != boost::wave::T_SEMICOLON && first != last)
				ss << first->get_value(), first++;

			object_name = ss.str();
		}


		ctx.first = cut_to_here;
		
		std::string final_msl;
		auto prefix = "temp.mx_bind.uniform";

		{
			std::stringstream ss;
			ss << "{\n";
			ss << prefix << ": " << object_to_generate << "\n";
			ss << prefix << ".n: \"" << object_name << "\"\n";
			ss << "}\n";
			final_msl = ss.str();
		}



		auto& shader_data = Context<MX::Graphic::Utils::ShaderData>::current();



		auto create_uniform = [=]() -> std::shared_ptr<UniformBase>
		{
			ScriptParser::ParseStringToScript(final_msl, Script::get());
			auto obj = Script::get().objectOrNull(prefix);
			if (!obj)
				return nullptr;

			return obj->to_object<Graphic::UniformBase>();
		};

		shader_data._uniformCreators.push_back(create_uniform);

	}

	std::string preprocess(const std::string& vertex_path, const std::map<std::string, std::string>& defines)
	{
		std::ifstream instream(MX::Paths::get().pathToResource(vertex_path));
		
		if (instream.fail())
			throw std::exception();
		
		



		std::string input(
			(std::istreambuf_iterator<char>(instream)),
			std::istreambuf_iterator<char>());

		if (!defines.empty())
		{
			std::string version;
			{
				auto pos = input.find('\n');
				if (pos != std::string::npos)
					version = input.substr(0, pos);

				std::string expected = "#version";
				if (version.compare(0, expected.size(), expected) != 0)
					version.clear();
			}
			
			if (version.empty())
				throw std::exception("Expected #version in shader");

			std::stringstream out;
			out << version << std::endl;

			for (auto &def : defines)
				out << "#define " << def.first << " " << def.second << std::endl << std::endl;

			
			out << "//" << input; //comment out first line (should be #version)

			input = out.str();
		}




		using lex_iterator_type = boost::wave::cpplexer::lex_iterator< boost::wave::cpplexer::lex_token<> >;
		using context_type = boost::wave::context< std::string::iterator, lex_iterator_type, boost::wave::iteration_context_policies::load_file_to_string, my_hooks >;


		auto file_name = boost::filesystem::path(vertex_path).filename().string();

		context_type ctx(input.begin(), input.end(), file_name.c_str());
		ctx.set_language(boost::wave::enable_single_line(ctx.get_language())); //if we don't do this, wave will throw exceptions if source doesn't end with newline

		ctx.add_sysinclude_path(MX::Paths::get().pathToResource("Shaders/include/").c_str());

		context_type::iterator_type first = ctx.begin();
		context_type::iterator_type last = ctx.end();

		
		std::stringstream ss;
		pp_context pp_ctx{ first, last, ss };

		while (first != last) {

			process(pp_ctx);
			if (first == last)
				break;

			auto &value = first->get_value();
			ss << value;
			++first;
		}

		auto parsed_shader = ss.str();
		return parsed_shader;
	}

}
}



MX::Graphic::Utils::ShaderData MX::Graphic::Utils::prepareShader(const std::string& vertex_path, const std::string& fragment_path, const std::map<std::string, std::string>& defines)
{
	ShaderData s;
	auto data_guard = Context<ShaderData>::Lock(s);

	ci::gl::GlslProgRef shader;
	try {
		auto vert_source = detail::preprocess(vertex_path, defines);
		auto fragment_source = detail::preprocess(fragment_path, defines);

		shader = ci::gl::GlslProg::create(vert_source.c_str(), fragment_source.c_str());
	}
	catch (ci::gl::GlslProgCompileExc &exc) {
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		std::wcout << L"- Shader compile error" << std::endl;
		std::wcout << L"V: " << convert.from_bytes(vertex_path) << L"F: " << convert.from_bytes(fragment_path) << std::endl;

		std::wstring error = convert.from_bytes(exc.what());
		std::wcout << error << std::endl << std::endl;

		//assert(false);
		return s;
	}
	catch (...) {
		std::wcout << L"Unable to load shader" << std::endl;
		assert(false);
		return s;
	}

	
	s.shader = shader;

	return s;
}


MX::Graphic::ShaderContainer::ShaderContainer(LScriptObject& script)
{
	script.load_property(_samplers, "Samplers");
	script.load_property(_shader, "Shader");

	if (_shader && _shader->shader())
	{
		auto& s = _shader->shader();
		_shader->bind();
		script.load_property(_uniforms, "Uniforms");
		_shader->unbind();
	}
}


void MX::Graphic::Utils::DrawPolygon(const cinder::Shape2d &path, const MX::Color &color)
{
	auto renderer = Graphic::CinderRenderer::get().Use();
	ci::gl::color(color.toColorA());
	ci::gl::draw(path);
}

void MX::Graphic::Utils::DrawPolygon(const cinder::Path2d &path, const MX::Color &color)
{
	auto renderer = Graphic::CinderRenderer::get().Use();
	ci::gl::color(color.toColorA());
	ci::gl::draw(path);
}



std::string MX::Graphic::Utils::ShapeToString(const cinder::Shape2d &path)
{
	if (path.getContours().size() == 0)
		return "0";

	std::stringstream ss;
	assert(path.getContours().size() <= 1);

	auto &contour = path.getContour(0);
	auto &points = contour.getPoints();

	ss << points.size() << " ";
	for (auto& point : points)
	{
		ss << point.x << " ";
		ss << point.y << " ";
	}

	return ss.str();
}

cinder::Shape2d MX::Graphic::Utils::StringToShape(const std::string &path)
{
	cinder::Shape2d shape;
	std::stringstream ss(path);
	int size = 0;
	ss >> size;

	for (int i = 0; i < size; i++)
	{
		glm::vec2 point;
		ss >> point.x;
		ss >> point.y;

		if (i == 0)
			shape.moveTo(point);
		else
			shape.lineTo(point);
	}

	if (size != 0)
		shape.close();

	return shape;
}


void operator & (cinder::Shape2d &path, MX::Scriptable::Variable&& var)
{
	if (MX::Scriptable::Variable::currentlySerializing())
	{
		std::string serialized = MX::Graphic::Utils::ShapeToString(path);
		serialized & var;
	}
	else
	{
		std::string serialized;
		serialized & var;
		path = MX::Graphic::Utils::StringToShape(serialized);
	}
}
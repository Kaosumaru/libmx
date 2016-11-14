#include "Shader.h"
#include "Program.h"
#include "game/resources/Resources.h"

using namespace MX;
using namespace MX::gl;

namespace MX
{
	namespace gl
	{
		Program createProgramFromFiles(const std::string& vertexShader, const std::string& fragmentShader, std::string& errorLog)
		{
			std::string vertex = MX::Resources::get().openTextFile(vertexShader);
			std::string fragment = MX::Resources::get().openTextFile(fragmentShader);
			return createProgram(vertex, fragment, errorLog);
		}

		Program createProgram(const std::string& vertexShader, const std::string& fragmentShader, std::string& errorLog)
		{
			Program program;

			Shader vertex;
			if (!vertex.Compile(vertexShader, Shader::Type::Vertex))
			{
				errorLog = vertex.infoLog();
				return program;
			}

			Shader fragment;
			if (!fragment.Compile(fragmentShader, Shader::Type::Fragment))
			{
				errorLog = fragment.infoLog();
				return program;
			}

			program.AttachShader(vertex);
			program.AttachShader(fragment);

			if (!program.Link())
			{
				errorLog = program.infoLog();
				return program;
			}

			return program;
		}
	}
}

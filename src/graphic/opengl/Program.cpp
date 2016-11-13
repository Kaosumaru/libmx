#include "Shader.h"
#include "Program.h"

using namespace MX;
using namespace MX::gl;

namespace MX
{
	namespace gl
	{
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

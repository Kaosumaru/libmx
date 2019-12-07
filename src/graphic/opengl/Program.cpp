#include "Program.h"
#include "Shader.h"
#include "game/resources/Resources.h"
#include "slang/ast.h"
#include "slang/lexer.h"
#include "slang/parser.h"
#include "slang/preprocessor.h"
#include "slang/print.h"
#include <sstream>

using namespace MX;
using namespace MX::gl;

namespace MX
{
namespace gl
{
    namespace
    {
        std::map<std::string, std::vector<slang::Token>> shaderCache;
    }

    const std::vector<slang::Token>& tokenizeShader(const std::string& shaderPath)
    {
        using namespace slang;

        auto it = shaderCache.find(shaderPath);
        if (it == shaderCache.end())
        {
            auto& tokens = shaderCache[shaderPath];
            auto shaderBody = MX::Resources::get().openTextFile(shaderPath);
            assert(!shaderBody.empty());
            Keywords keys;
            keys.add_all_keywords();

            std::istringstream is { shaderBody };
            Logger logger(shaderPath, true);
            Lexer lexer(is, keys, logger);
            Preprocessor pp(lexer, logger, [](int ver, Profile p) {
                std::cout << "GLSL version: " << ver << " " << p << std::endl;
                return true;
            });
            pp.register_builtin_macros();

            pp.register_macro("include", Macro({ { "path", 0 } }, [&lexer](const std::vector<Macro::Arg>& args) {
                auto& pathArg = args[0];
                std::ostringstream p;
                for (auto& t : pathArg)
                    p << t;
                auto path = p.str();

                return tokenizeShader(path);
            }));

            Token tok;
            while (true)
            {
                tok = pp.preprocess();
                assert(pp.error_count() == 0);
                if (tok.type() == Token::END)
                    break;
                tokens.push_back(tok);
            }
            return tokens;
        }

        return it->second;
    }

    std::string preprocessShader(const std::string& shaderPath)
    {
        std::ostringstream ss;
        auto& tokens = tokenizeShader(shaderPath);

        for (auto& t : tokens)
            ss << t << " ";

        return ss.str();
    }

    Program::pointer createProgramFromFiles(const std::string& vertexShader, const std::string& fragmentShader, std::string& errorLog, bool allowDebugReload)
    {
        auto vertex = preprocessShader(vertexShader);
        auto fragment = preprocessShader(fragmentShader);

        bool error = false;
        if (vertex.empty())
        {
            errorLog = "Shader file at " + vertexShader + " not parsed correctly!";
            error = true;
        }

        if (fragment.empty())
        {
            errorLog = "Shader file at " + fragmentShader + " not parsed correctly!";
            error = true;
        }

        if (error)
            return nullptr;

        auto program = createProgram(vertex, fragment, errorLog);
#ifdef _DEBUG
        if (allowDebugReload && program)
        {
            program->InitializeDebugReload(vertexShader, fragmentShader);
        }
#endif

        return program;
    }

    Program::pointer createProgram(const std::string& vertexShader, const std::string& fragmentShader, std::string& errorLog)
    {
        Shader vertex;
        if (!vertex.Compile(vertexShader, Shader::Type::Vertex))
        {
            errorLog = vertex.infoLog();
            return nullptr;
        }

        Shader fragment;
        if (!fragment.Compile(fragmentShader, Shader::Type::Fragment))
        {
            errorLog = fragment.infoLog();
            return nullptr;
        }

        Program::pointer program = std::make_shared<Program>();
        program->AttachShader(vertex);
        program->AttachShader(fragment);

        if (!program->Link())
        {
            errorLog = program->infoLog();
            return nullptr;
        }

        return program;
    }

#ifdef _DEBUG
    void Program::InitializeDebugReload(const std::string& vertexPath, const std::string& fragmentPath)
    {
        _vertexPath = vertexPath;
        _fragmentPath = fragmentPath;
        _fileObserver = FileObserver::Create();
        _fileObserver->RegisterPath(Paths::get().pathToResource(_vertexPath));
        _fileObserver->RegisterPath(Paths::get().pathToResource(_fragmentPath));
        _fileObserver->onFilesChanged.static_connect([&]() { DebugReload(); });
    }

    void Program::DebugReload()
    {
        shaderCache.clear();
        std::string errorLog;
        Program::pointer program = createProgramFromFiles(_vertexPath, _fragmentPath, errorLog, false);

        std::cout << "Reloading shader " << _fragmentPath << "@" << _vertexPath << std::endl;
        if (!program)
        {
            std::cout << "Failed to reload shared" << std::endl;
            std::cout << errorLog << std::endl;
            return;
        }

        std::swap(_ownerID, program->_ownerID);
        std::swap(_object, program->_object);
    }
#endif

}
}

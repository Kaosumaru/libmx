#pragma once
#include <string>

namespace MX
{
	class Script;

	class ScriptParser
	{
	public:
		ScriptParser(Script &Script);
		virtual bool Parse() = 0;

		static void ParseFileToScript(const std::string& path, Script &sc);

#ifdef WIPMSL
		static void ParseStringToScript(const std::string& data, Script &sc);
#endif
	protected:
		Script &_Script;
	};
}


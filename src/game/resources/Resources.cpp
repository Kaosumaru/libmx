#include "Resources.h"
#include <sstream>
#include <fstream>


namespace MX
{

	std::string Resources::openTextFile(const std::string& path)
	{
		std::string input;

		std::ifstream instream(Paths::get().pathToResource(path));

		if (instream.fail())
			return input;

		input = { (std::istreambuf_iterator<char>(instream)),
			std::istreambuf_iterator<char>() };

		return input;
	}

}
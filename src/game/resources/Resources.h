#pragma once
#include <string>
#include "Paths.h"

namespace MX
{

class Resources : public Singleton<Resources>
{
public:
	std::string openTextFile(const std::string& path);
protected:

};

}

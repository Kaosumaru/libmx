#include "MXScriptableVariable.h"
#include "JSON/json.h"
#include "Game/Resources/MXPaths.h"
#include <boost/format.hpp>
#include <memory>
#include <regex>
#include <iterator>
#include <sstream>
#include <iostream>
#include "Utils/MXMakeShared.h"
#include "../MXScriptClassParser.h"
#include "../MXScript.h"


using namespace MX;



void operator & (MX::Vector2 &t, MX::Scriptable::Variable&& var)
{
	t.x & var("x");
	t.y & var("y");
}



Scriptable::Variable::Variable(const std::string &label)
{
    _fullPath = label;
	size_t size = label.find_last_of('.');
	if (size != std::string::npos && size > 0)
		_path = label.substr(0, size);
}
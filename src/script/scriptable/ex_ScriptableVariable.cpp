#include "../MXScript.h"
#include "../MXScriptClassParser.h"
#include "Game/Resources/MXPaths.h"
#include "MXScriptableVariable.h"
#include "Utils/MXMakeShared.h"
#include "JSON/json.h"
#include <boost/format.hpp>
#include <iostream>
#include <iterator>
#include <memory>
#include <regex>
#include <sstream>

using namespace MX;

void operator&(MX::Vector2& t, MX::Scriptable::Variable&& var)
{
    t.x& var("x");
    t.y& var("y");
}

Scriptable::Variable::Variable(const std::string& label)
{
    _fullPath = label;
    size_t size = label.find_last_of('.');
    if (size != std::string::npos && size > 0)
        _path = label.substr(0, size);
}
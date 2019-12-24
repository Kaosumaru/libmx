#include "Parser.h"
#include "RPN/Function.h"
#include "RPN/Utils.h"
#include "script/ScriptClassParser.h"
#include "utils/Random.h"
#include "utils/Time.h"
#include "utils/Log.h"
#include <map>

#if WIP
#include <boost/format.hpp>
#endif

using namespace RPN;

namespace RPN
{

bool rule_variable(Parser::Context& context)
{
    auto peek = context.input.peek();
    if (peek != '&')
        return false;

    context.input.get();

    int parenthesis_count = 0;
    auto charsAllowedInVarName = [&](char c, int index) {
        bool allowed = isalnum(c) || (index != 0 && c == '.') || c == '_';
        if (allowed)
            return true;
        if (c == ')' && --parenthesis_count >= 0)
            return true;
        if (c == '(' && index == 0)
        {
            parenthesis_count++;
            return true;
        }
        return false;
    };

    std::string str = eat_string_in_stream(context.input, charsAllowedInVarName);

    auto& ret = MX::Script::objectOrNull(str);

    class ScriptVariable : public Token
    {
    public:
        ScriptVariable(const MX::Scriptable::Value::pointer& value)
            : _value(value)
        {
        }

        bool constant() override { return false; }
        float value() override { return *_value; }
        std::string stringValue() override { return _value->atext(); }

    protected:
        MX::Scriptable::Value::pointer _value;
    };

    if (!ret)
    {
        spdlog::error("When parsing RPN script, not found pointer: &{}", str);
        context.AddToken(new Value(0.0f));
    }
    else
    {
        context.AddToken(new ScriptVariable(ret));
    }

    return true;
}

}

MX::RPNParser::RPNParser()
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;

        Functions::AddStatelessLambda("time.now", []() -> float { return MX::Time::Timer::current().total_seconds(); });

        {

            { Functions::AddStatelessLambda("random.float", [](float a, float b) { return Random::randomRange(std::make_pair(a, b)); });
    }
}

{
    RPN::Functions::AddStatelessLambda("rgb", [](float r, float g, float b) -> float {
        auto int_color = Color { r, g, b }.toIntARGB();
        return reinterpret_cast<float&>(int_color);
    });

    RPN::Functions::AddStatelessLambda("rgba", [](float r, float g, float b, float a) -> float {
        auto int_color = Color { r, g, b, a }.toIntARGB();
        return reinterpret_cast<float&>(int_color);
    });

    RPN::Functions::AddStatelessLambda("color_add", [](float c1, float c2) -> float {
        auto cx1 = MX::Color { reinterpret_cast<unsigned int&>(c1) };
        auto cx2 = MX::Color { reinterpret_cast<unsigned int&>(c2) };

        cx1 = cx1 + cx2;

        auto int_color = cx1.toIntARGB();
        return reinterpret_cast<float&>(int_color);
    });

#if WIP
    Functions::AddLambda("string.format", [](const std::vector<TokenPtr>& tokens) -> std::string {
        if (tokens.empty())
            return "";

        auto form = boost::format(tokens[0]->stringValue());

        for (unsigned i = 1; i < tokens.size(); i++)
        {
            auto& token = tokens[i];

            if (token->returnType() == Token::VariableType::String)
                form % token->stringValue();
            else
                form % token->value();
        }

        return form.str();
    });
#endif
}
}

_rules.push_back(rule_variable);
}
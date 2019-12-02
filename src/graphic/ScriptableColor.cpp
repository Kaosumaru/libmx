#include "ScriptableColor.h"

using namespace MX;

void ScriptableColor::SetValue(const Scriptable::Value::pointer& value)
{
    _value = value;
    _constant = true;
    if (value)
        _constant = value->member()->isConstant();
    if (_constant)
        Recalculate();
}

void ScriptableColor::SetValues(const ColorsValue& values)
{
    _channels = values;
    _constant = true;

    for (int i = 0; i < 4; i++)
        if (_channels[i])
            _constant &= _channels[i]->member()->isConstant();

    if (_constant)
        Recalculate();
}

void ScriptableColor::Recalculate()
{
    if (_value)
    {
        float color = *_value;
        unsigned int int_c = reinterpret_cast<unsigned int&>(color);
        _color = MX::Color { int_c };
        return;
    }
    for (int i = 0; i < 4; i++)
        _color[i] = _channels[i] ? *(_channels[i]) : 1.0f;
}
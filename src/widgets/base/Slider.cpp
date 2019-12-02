#include "Slider.h"
#include "application/Window.h"
#include "collision/area/EventsToCollisions.h"

using namespace MX;
using namespace Graphic;
using namespace Widgets;

namespace MX
{
namespace Widgets
{
    namespace Strategy
    {

        class SlideButton : public Button
        {
        public:
            SlideButton(Slider& slider)
                : _slider(slider)
            {
            }

        protected:
            void OnTouchMove()
            {
                if (!buttonWidget().parentWidget())
                    return;

                float value = _slider.valueForPosition(_touch->point().x - buttonWidget().parentWidget()->absolute_position().x);
                _slider.SetValue(value);
            }

            void OnTouchBegin() override
            {
                Button::OnTouchBegin();
                _touch->on_move.connect(std::bind(&SlideButton::OnTouchMove, this), shared_from_this());
            }

            void OnTouchEnd() override
            {
                Button::OnTouchEnd();
            }

            Slider& _slider;
        };

    }
}
}

Slider::Slider()
{
    AddWidget(_knob = std::make_shared<ButtonWidget>());

    _knob->AddStrategy(std::make_shared<Strategy::SlideButton>(*this));

    auto sig = [this](float w, float h) { onSizeOrKnobChanged(); };
    on_size_changed.connect(sig, this);
    _knob->on_size_changed.connect(sig, this);
}

void Slider::SetValue(float v)
{
    if (_value == v)
        return;
    _value = v;
    onSizeOrKnobChanged();
    onValueChanged(value());
}

float Slider::positionForValue(float value)
{
    return std::min(std::max(_availableLength * value, 0.0f), _availableLength);
}
float Slider::valueForPosition(float position)
{
    return std::min(std::max(position / _availableLength, 0.0f), 1.0f);
}

void Slider::onSizeOrKnobChanged()
{
    _availableLength = Width() - _knob->Width();
    _knob->SetPosition(positionForValue(_value), _knob->position().y);
}

void Slider::onValueChanged(float value)
{
}

SliderMinMax::SliderMinMax()
{
}

float SliderMinMax::positionForValue(float value)
{
    value -= _minValue;
    value /= (_maxValue - _minValue);
    return Slider::positionForValue(value);
}

float SliderMinMax::valueForPosition(float position)
{
    auto value = Slider::valueForPosition(position);
    value = _minValue + (_maxValue - _minValue) * value;
    return value;
}

void SliderMinMax::SetMinValue(float value)
{
    if (_minValue == value)
        return;
    _minValue = value;
    onSizeOrKnobChanged();
}

void SliderMinMax::SetMaxValue(float value)
{
    if (_minValue == value)
        return;
    _maxValue = value;
    onSizeOrKnobChanged();
}

SliderMinMaxForVariable::SliderMinMaxForVariable(SignalizingVariable<float>& variable)
    : _variable(variable)
{
    using namespace std::placeholders;
    _data = std::make_shared<int>();
    _variable.onValueChanged.connect(std::bind(&SliderMinMaxForVariable::onVariableChanged, this, _1), _data);
    SetValue(_variable);
}

void SliderMinMaxForVariable::onValueChanged(float value)
{
    _variable = value;
}

void SliderMinMaxForVariable::onVariableChanged(float value)
{
    SetValue(value);
}
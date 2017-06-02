#pragma once
#include "widgets/Widget.h"
#include "utils/SignalizingVariable.h"

namespace MX
{
namespace Widgets
{


	class Slider : public Widget, public MX::SignalTrackable
	{
	public:
		Slider();

		virtual float value() { return _value; }
		virtual void SetValue(float value);

		virtual float positionForValue(float value);
		virtual float valueForPosition(float position);

		const std::shared_ptr<ButtonWidget> &knob() { return _knob; }
	protected:
		virtual void onValueChanged(float value);

		void onSizeOrKnobChanged();

		float _availableLength = 0.0f;
		SignalizingVariable<float> _value = 0.0f;

		std::shared_ptr<ButtonWidget> _knob;
	};


	class SliderMinMax : public Slider
	{
	public:
		SliderMinMax();

		void SetMinValue(float value);
		void SetMaxValue(float value);
	protected:
		float positionForValue(float value) override;
		float valueForPosition(float position) override;

		float _minValue = 0.0f;
		float _maxValue = 1.0f;
	};


	class SliderMinMaxForVariable : public SliderMinMax
	{
	public:
		SliderMinMaxForVariable(SignalizingVariable<float> &variable);

	protected:
		void onValueChanged(float value) override;
		void onVariableChanged(float value);

		std::shared_ptr<int> _data;
		SignalizingVariable<float> &_variable;
	};

}
}

#pragma once
#include "Widget.h"
#ifdef WIPFONT
#include "graphic/Fonts/Font.h"
#endif
#include "Strategies/DrawableStrategy.h"

namespace MX
{
namespace Widgets
{



class Label : public ButtonWidget
{
public:
	Label(const std::wstring &text);
	Label();

#ifdef WIPFONT
	void SetHTML(bool html) { _textStrategy.text().SetHTML(html); }
	void SetText(const std::wstring &text) { _textStrategy.text().SetText(text); }
	const auto &text() const { return _textStrategy.text().text(); }
#else
	void SetHTML(bool html) { }
	void SetText(const std::wstring &text) {  }
#endif

protected:
#ifdef WIPFONT
	Strategy::Drawable::Text _textStrategy;
#endif
};


class AutoLabel : public ButtonWidget
{
public:
	using StringBuilder = std::function<std::wstring(void)>;

	AutoLabel();

#ifdef WIPFONT
	void SetHTML(bool html) { _textStrategy->text().SetHTML(html); }
	void SetStringBuilder(const StringBuilder& builder) { _textStrategy->SetStringBuilder(builder); }
#endif

	template<typename ...Args>
	void connect_signal(MX::SimpleSignal<Args...>& signal)
	{
		signal.connect(std::bind(&AutoLabel::MarkAsDirty, this), shared_from_this());
	}
	

	template<typename ...Args>
	void connect_signal(MX::Signal<void (Args...)>& signal)
	{
		signal.connect(std::bind(&AutoLabel::MarkAsDirty, this), shared_from_this());
	}
    
    template<typename T1>
    void connect_signals(T1 &t1)
    {
        connect_signal(t1);
    }
    
	template<typename T1, typename ...T>
	void connect_signals(T1 &t1, T&... t)
	{
		connect_signal(t1);
		connect_signals(t...);
	}


	void MarkAsDirty()
	{
#ifdef WIPFONT
		_textStrategy->MarkAsDirty();
#endif
	}

protected:
#ifdef WIPFONT
	std::shared_ptr<Strategy::Drawable::AutoText> _textStrategy;
#endif
};




class LabelButton : public Label
{
public:
	LabelButton(const std::wstring &text);
	LabelButton();


protected:
	Strategy::Button _buttonStrategy;
};


class FPSLabel : public Label
{
public:
	FPSLabel();

	void Run() override;
protected:
	float fps();
	float defaultTime() { return 0.5f; }

	Time::ManualStopWatchAbsolute   _stopWatch;
	float sum = 0.0f;
	int count = 0;
	Time::SimpleTimer _timer;
};

using LabelPtr = std::shared_ptr<Widgets::Label>;
using LabelButtonPtr = std::shared_ptr<Widgets::LabelButton>;



}

}



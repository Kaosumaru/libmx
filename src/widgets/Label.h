#pragma once
#include "Widget.h"
#ifdef WIPFONT
#include "Graphic/Fonts/Font.h"
#endif
#include "Strategies/DrawableStrategy.h"

namespace MX
{
namespace Widgets
{


#ifdef WIPFONT
class Label : public ButtonWidget
{
public:
	Label(const std::wstring &text);
	Label();

	void SetHTML(bool html) { _textStrategy.text().SetHTML(html); }
	void SetText(const std::wstring &text) { _textStrategy.text().SetText(text); }

	const auto &text() const { return _textStrategy.text().text(); }
protected:
	Strategy::Drawable::Text _textStrategy;
};


class AutoLabel : public ButtonWidget
{
public:
	using StringBuilder = std::function<std::wstring(void)>;

	AutoLabel();

	void SetHTML(bool html) { _textStrategy->text().SetHTML(html); }
	void SetStringBuilder(const StringBuilder& builder) { _textStrategy->SetStringBuilder(builder); }

	template<typename ...Args>
	void connect_signal(MX::SimpleSignal<Args...>& signal)
	{
		signal.connect(boost::bind(&AutoLabel::MarkAsDirty, this), shared_from_this());
	}
	

	template<typename ...Args>
	void connect_signal(MX::Signal<void (Args...)>& signal)
	{
		signal.connect(boost::bind(&AutoLabel::MarkAsDirty, this), shared_from_this());
	}

	template<typename T>
	void connect_signal(default_signal<T>& signal)
	{
		signal.connect(boost::bind(&AutoLabel::MarkAsDirty, this));
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
		_textStrategy->MarkAsDirty();
	}

protected:
	std::shared_ptr<Strategy::Drawable::AutoText> _textStrategy;
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

typedef std::shared_ptr<Widgets::Label> LabelPtr;
typedef std::shared_ptr<Widgets::LabelButton> LabelButtonPtr;
#endif


}

}



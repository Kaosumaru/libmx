#ifndef MXDRAWABLESTRATEGY
#define MXDRAWABLESTRATEGY
#include "Strategy.h"
#include "Graphic/Images/Image.h"
#include "Graphic/Fonts/Font.h"

namespace MX
{
namespace Widgets
{

	namespace Strategy
	{
		namespace Drawable
		{
			class Drawable : public Strategy
			{
			public:
				bool drawable() override { return true; }
			};
		}
	}


class TextData : public ScopeSingleton<TextData>
{
public:
	const std::wstring& text() const { return _text; }
	void SetText(const std::wstring& text) { if (_text == text) return; _dirty = true; _text = text; }

	const Graphic::Font::pointer& font() const { return _font; }
	void SetFont(const Graphic::Font::pointer& font) { if (_font == font) return;  _dirty = true; _font = font; }

	const unsigned width() const { return _width; }
	void SetWidth(unsigned width) { if (_width == width) return; _dirty = true; _width = width; }


	const std::shared_ptr<Graphic::Image>& textImage() { UpdateTextImage(); return _textImage; }

	bool dirty() const { return _dirty; }

	void UpdateTextImage();

	void SetHTML(bool html) { if (_html == html) return; _html = html; _dirty = true; }
	bool HTML() const { return _html; }

	unsigned actualWidth() 
	{ 
		auto tex = textImage();
		if (tex)
			return tex->Width();
		return 0;
	}
protected:
	void SetTextImage(const std::shared_ptr<Graphic::Image>& image) { _textImage = image; _dirty = false; }

	std::wstring _text;
	Graphic::Font::pointer _font;
	unsigned _width = 0;
	unsigned _textureWidth = 0;

	bool _html = false;
	bool _dirty = false;
	std::shared_ptr<Graphic::Image> _textImage;

};


namespace Strategy
{
	namespace Drawable
	{
		class Image : public Drawable
		{
		public:
			Image() {}
			Image(const std::shared_ptr<Graphic::Image>& image) { SetImage(image); }

			void BeforeDraw() override;
			void AfterDraw() override;

			const auto &image() const { return _image; }
			void SetImage(const std::shared_ptr<Graphic::Image>& image) { _image = image; }
		protected:
			Graphic::Image *_old = nullptr; //TODO remove
			std::shared_ptr<Graphic::Image> _image;
		};

		class Text : public Drawable
		{
		public:
			void BeforeDraw() override;
			void AfterDraw() override;

			auto &text() { return _textData; }
			const auto &text() const { return _textData; }
		protected:
			TextData *_old = nullptr; //TODO remove
			TextData _textData;
		};


		class AutoText : public Text
		{
		public:
			using StringBuilder = std::function<std::wstring(void)>;

			AutoText() {}
			AutoText(const StringBuilder& builder)
			{
				_stringBuilder = builder;
				Build();
			}

			bool runnable() override { return true; }
			bool Run() override
			{
				if (_dirty)
				{
					_dirty = false;
					Build();
				}
				return true;
			}

			void SetStringBuilder(const StringBuilder& builder) { _stringBuilder = builder; Build(); }
			void Build() { if (_stringBuilder) text().SetText(_stringBuilder()); }


			template<typename ...Args>
			void connect_signal(MX::SimpleSignal<Args...>& signal)
			{
				signal.connect(boost::bind(&AutoText::MarkAsDirty, this), shared_from_this());
			}


			template<typename ...Args>
			void connect_signal(MX::Signal<void(Args...)>& signal)
			{
				signal.connect(boost::bind(&AutoText::MarkAsDirty, this), shared_from_this());
			}

			template<typename T>
			void connect_signal(default_signal<T>& signal)
			{
				signal.connect(boost::bind(&AutoText::MarkAsDirty, this));
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
				_dirty = true;
			}
		protected:
			bool _dirty = false;
			StringBuilder _stringBuilder;

		};
	}
}


//-------Progress

class ProgressData : public ScopeSingleton<ProgressData>
{
public:
	ProgressData() {}
	ProgressData(float p) : _progress(p) {}

	void Set(float p) { _progress = p; }
	auto value() { return _progress; }
protected:
	float _progress = 0.0f;
};


namespace Strategy
{
	namespace Drawable
	{
		class Progress : public Drawable
		{
		public:
			void BeforeDraw() override;
			void AfterDraw() override;

			auto &progress() { return _progressData; }
		protected:
			ProgressData *_old = nullptr; //TODO remove
			ProgressData _progressData;
		};



		class ProgressFromSignalizing : public Progress
		{
		public:
			ProgressFromSignalizing();
			ProgressFromSignalizing(SignalizingVariable<float>& value, SignalizingVariable<float>& max);

			void SetVariables(SignalizingVariable<float>* value, SignalizingVariable<float>* max);

		protected:
			void OnValueChanged();

			std::shared_ptr<int> _data;
			SignalizingVariable<float> *_value;
			SignalizingVariable<float> *_max;
			boost::signals2::connection _value_connection;
			boost::signals2::connection _max_connection;
		};
	}
}

}
}

#endif

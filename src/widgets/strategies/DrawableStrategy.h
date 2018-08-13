#ifndef MXDRAWABLESTRATEGY
#define MXDRAWABLESTRATEGY
#include "Strategy.h"
#include "graphic/images/Image.h"
#include "graphic/fonts/Font.h"
#include "graphic/fonts/BitmapFont.h"
#include "utils/SignalizingVariable.h"

namespace MX
{
class SpriteScene;

namespace Widgets
{

	namespace Strategy
	{
		namespace Drawable
		{
			class Drawable : public MX::Widgets::Strategy::Strategy
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
	void SetFont(const Graphic::Font::pointer& font) { if (_font == font && !_bitmapFont) return;  _bitmapFont = nullptr;  _dirty = true; _font = font; }
	void SetBitmapFont(const Graphic::BitmapFontSized::pointer& font) { if (_bitmapFont == font && !_font) return;  _font = nullptr;  _dirty = true; _bitmapFont = font; }

	const unsigned width() const { return _width; }
	void SetWidth(unsigned width) { if (_width == width) return; _dirty = true; _width = width; }


	bool dirty() const { return _dirty; }

	void UpdateTextImage();

	void SetHTML(bool html) { if (_html == html) return; _html = html; _dirty = true; }
	bool HTML() const { return _html; }

	unsigned actualWidth();
	unsigned actualHeight();

	bool isBitmapFont() { return _bitmapFont != nullptr; }
	const std::shared_ptr<Graphic::Image>& textImage() { UpdateTextImage(); return _textImage; }
	const Graphic::RenderQueue& renderQueue()          { UpdateTextImage(); return _renderQueue; }
protected:
	void SetTextImage(const std::shared_ptr<Graphic::Image>& image) { _textImage = image; _dirty = false; }

	std::wstring _text;
	Graphic::Font::pointer _font;
	std::shared_ptr<Graphic::Image> _textImage;

	Graphic::BitmapFontSized::pointer _bitmapFont;
	Graphic::RenderQueue         _renderQueue;

	unsigned _width = 0;
	unsigned _textureWidth = 0;

	bool _html = false;
	bool _dirty = false;
	

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

		class SceneHolder : public Drawable
		{
		public:
			using SpriteScenePtr = std::shared_ptr<MX::SpriteScene>;
			SceneHolder() {}
			SceneHolder(const SpriteScenePtr& scene){ SetScene(scene);	}

			bool Run() override;

			void BeforeDraw() override;
			void AfterDraw() override;

			const auto &scene() const { return _scene; }
			void SetScene(const SpriteScenePtr& scene);

			bool runnable() override { return true; }
		protected:
			MX::SpriteScene *_old = nullptr; //TODO remove
			SpriteScenePtr _scene;
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
				signal.connect(std::bind(&AutoText::MarkAsDirty, this), shared_from_this());
			}


			template<typename ...Args>
			void connect_signal(MX::Signal<void(Args...)>& signal)
			{
				signal.connect(std::bind(&AutoText::MarkAsDirty, this), shared_from_this());
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
			Connection _value_connection;
			Connection _max_connection;
		};
	}
}

}
}

#endif

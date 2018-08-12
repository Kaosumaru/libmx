#include "HTMLRendererQueue.h"
#include "graphic/images/Surface.h"
#include "graphic/images/TextureImage.h"
#include <memory>
#include <codecvt>
#include <sstream>
#include <string>
#include "game/resources/Paths.h"

#include "graphic/Blender.h"
#include "graphic/fonts/Font.h"

#include "MasterCSS.h"
#include "include/litehtml.h"


#include "graphic/opengl/Utils.h"



using namespace MX;

using namespace std;

using namespace litehtml;


namespace
{

	class ft_html_container_base : public litehtml::document_container
	{
	public:
		ft_html_container_base(const HTMLRendererQueue::Options& options = {}) : _options(options)
		{

		}


		~ft_html_container_base()
		{

		}

		void SetDefaultFont(const std::shared_ptr<Graphic::BitmapFont> &font)
		{
			_defaultFont = font;
		}

		void SetDefaultFontBold(const std::shared_ptr<Graphic::BitmapFont> &font)
		{
			_defaultFontBold = font;
		}

		struct FontInfo
		{
			Graphic::BitmapFont::pointer         _font;
			float                                _scale;
			int                                  _weight;
			int                                  _size;
			int                                  _id = 0;

			void FillMetric(litehtml::font_metrics* fm)
			{
				auto X = _font->character('X');
				auto x = _font->character('x');

				fm->ascent = _font->baseline();
				fm->height = _font->lineHeight();
				fm->descent = _font->lineHeight() - _font->baseline();
				fm->x_height = x->info()->height;
				fm->draw_spaces = false;

				fm->ascent *= _scale;
				fm->height *= _scale;
				fm->descent *= _scale;
				fm->x_height *= _scale;

			}
		};

		auto& AddFont(const Graphic::BitmapFont::pointer& font, int weight, int size)
		{
			FontInfo info;
			info._font = font;
			info._weight = weight;
			info._size = size;

			info._scale = font->scaleForSize(size);
			info._id = _fonts.size();
			_fonts.push_back(info);

			return _fonts.back();
		}

		auto& fontForID(uint_ptr hFont)
		{
			return _fonts[hFont];
		}
	protected:
		Graphic::BitmapFont::pointer _defaultFont;
		Graphic::BitmapFont::pointer _defaultFontBold;

		std::vector<FontInfo> _fonts;
		const HTMLRendererQueue::Options& _options;
	};

	class ft_html_container_font : public virtual ft_html_container_base
	{
	public:


		uint_ptr create_font(const tchar_t* faceName, int size, int weight, font_style italic, unsigned int decoration, litehtml::font_metrics* fm) override
		{
			auto& f = AddFont(_defaultFont, weight, size);
			f.FillMetric(fm);
			return f._id;
		}

		void delete_font(uint_ptr hFont) override
		{
			//TODO
		}

		int	text_width(const tchar_t* text, uint_ptr hFont) override
		{
			auto font = fontForID(hFont);
			return font._font->MeasureText(text, font._scale);
		}

		void draw_text(uint_ptr hdc, const tchar_t* text, uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override
		{
			Graphic::RenderQueue &queue = *((Graphic::RenderQueue*)hdc);
			int weight = (int)hFont;
			auto font = fontForID(hFont);

			font._font->QueueText(queue, text, {pos.x, pos.y}, font._scale);
		}

		int	pt_to_px(int pt) override
		{
			return pt;
		}

		int	get_default_font_size() const override
		{
			if (_options.default_size != 0.0f) return _options.default_size;
			return _defaultFont ? _defaultFont->size() : 24;
		}
		const tchar_t* get_default_font_name() const override
		{
			return _t("Arial");
		}
	protected:

	};


	class ft_html_container_image : public virtual ft_html_container_base
	{
	public:
		void draw_list_marker(uint_ptr hdc, const litehtml::list_marker& marker) override
		{

		}

		void load_image(const tchar_t* src, const tchar_t* baseurl, bool redraw_on_ready) override
		{

		}

		void get_image_size(const tchar_t* src, const tchar_t* baseurl, litehtml::size& sz) override
		{

		}

		void draw_background(uint_ptr hdc, const litehtml::background_paint& bg) override
		{

		}

		void draw_borders(uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) override
		{

		}
	};


	class ft_html_container_other : public virtual ft_html_container_base
	{
	public:
		void set_caption(const tchar_t* caption) override
		{

		}

		void set_base_url(const tchar_t* base_url) override
		{

		}

		void link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) override
		{

		}

		void on_anchor_click(const litehtml::tchar_t* url, const litehtml::element::ptr& el) override
		{

		}

		void set_cursor(const tchar_t* cursor) override
		{

		}

		void transform_text(litehtml::tstring& text, litehtml::text_transform tt) override
		{

		}

		void import_css(tstring& text, const tstring& url, tstring& baseurl) override
		{

		}

		void set_clip(const litehtml::position& pos, const border_radiuses& bdr_radius, bool valid_x, bool valid_y) override
		{

		}

		void del_clip() override
		{

		}

		void get_client_rect(litehtml::position& client) const override
		{

		}

		std::shared_ptr<litehtml::element> create_element(const litehtml::tchar_t *tag_name, const litehtml::string_map &attributes, const std::shared_ptr<litehtml::document> &doc) override
		{
			return nullptr;
		}

		void get_media_features(litehtml::media_features& media) const override
		{

		}

		void get_language(litehtml::tstring& language, litehtml::tstring & culture) const override
		{
		}
	};

	class ft_html_container : public ft_html_container_font, public ft_html_container_image, public ft_html_container_other
	{
	public:
		ft_html_container(const HTMLRendererQueue::Options& options) : ft_html_container_base(options)
		{

		}


	};
}

Graphic::RenderQueue HTMLRendererQueue::Render(const char* str, float width, const std::shared_ptr<Graphic::BitmapFont>& defaultFont, const Options& options)
{
	static litehtml::context ctx;
	static bool initialized = false;
	if (!initialized)
	{
		initialized = true;
		ctx.load_master_stylesheet(HTMLUtils::mxmaster_css().c_str());
	}

	ft_html_container painter {options};

	//specific
	painter.SetDefaultFont(defaultFont);
	//end

	Graphic::RenderQueue queue;

	auto document = document::createFromString(str, &painter, &ctx);
	document->render(width);

	int w = document->render(width);
	int h = document->height();

	auto clip = litehtml::position(0, 0, w, h);
	document->draw((uint_ptr)&queue, 0, 0, &clip);

	queue.SetBounds({(float)w, (float)h});

	return queue;
}

Graphic::RenderQueue HTMLRendererQueue::Render(const wchar_t* str, float width, const std::shared_ptr<Graphic::BitmapFont>& defaultFont, const Options& options)
{
	auto tstr = wideToUTF(str);
	return Render(tstr.c_str(), width, defaultFont, options);
}
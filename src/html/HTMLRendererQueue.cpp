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
		ft_html_container_base()
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
	protected:
		std::shared_ptr<Graphic::BitmapFont> _defaultFont;
		std::shared_ptr<Graphic::BitmapFont> _defaultFontBold;
	};

	class ft_html_container_font : public virtual ft_html_container_base
	{
	public:

		auto& fontForWeight(int weight)
		{
			if (weight > 400 && _defaultFontBold)
				return _defaultFontBold;
			return _defaultFont;
		}

		uint_ptr create_font(const tchar_t* faceName, int size, int weight, font_style italic, unsigned int decoration, litehtml::font_metrics* fm) override
		{
			auto font = fontForWeight(weight);
			auto X = font->character('X');
			auto x = font->character('x');

			fm->ascent = X->info()->height - x->info()->height;
			fm->height = font->baseline() + x->info()->height; //fake
			fm->descent = font->baseline();
			fm->x_height = x->info()->height;
			fm->draw_spaces = false;

			return (uint_ptr)weight;
		}

		void delete_font(uint_ptr hFont) override
		{
			//TODO
		}

		int	text_width(const tchar_t* text, uint_ptr hFont) override
		{
			int weight = (int)hFont;
			auto font = fontForWeight(weight);
			return font->MeasureText(text);
		}

		void draw_text(uint_ptr hdc, const tchar_t* text, uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override
		{
			Graphic::RenderQueue &queue = *((Graphic::RenderQueue*)hdc);
			int weight = (int)hFont;
			auto font = fontForWeight(weight);

			font->QueueText(queue, text, {pos.x, pos.y}, 1.0f);
		}

		int	pt_to_px(int pt) override
		{
			return pt;
		}

		int	get_default_font_size() const override
		{
			return 16;
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



	};
}

Graphic::RenderQueue HTMLRendererQueue::Render(const char* str, float width, const std::shared_ptr<Graphic::BitmapFont>& defaultFont)
{
	static litehtml::context ctx;
	static bool initialized = false;
	if (!initialized)
	{
		initialized = true;
		ctx.load_master_stylesheet(HTMLUtils::mxmaster_css().c_str());
	}

	ft_html_container painter;

	//specific
	painter.SetDefaultFont(defaultFont);
	//end

	Graphic::RenderQueue queue;

	auto document = document::createFromString(str, &painter, &ctx);
	document->render(width);

	int w = document->width(), h = document->height();

	auto clip = litehtml::position(0, 0, w, h);
	document->draw((uint_ptr)&queue, 0, 0, &clip);

	
	return queue;
}
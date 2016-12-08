#include "HTMLRendererFreetype.h"
#include "graphic/images/Surface.h"
#include "graphic/images/TextureImage.h"
#include <memory>
#include <codecvt>
#include <sstream>
#include <string>
#include "game/resources/Paths.h"

#include "graphic/Blender.h"
#include "graphic/fonts/Font.h"


#include "include/litehtml.h"
#include "MasterCSS.h"

#include "graphic/fonts/freetype/Freetype.h"

using namespace MX;

using namespace std;

using namespace litehtml;






class ft_html_container_base : public litehtml::document_container
{
public:
	ft_html_container_base()
	{

	}


	~ft_html_container_base()
	{

	}

	void SetDefaultFont(const std::shared_ptr<Graphic::Face> &font)
	{
		_defaultFont = font;
	}
protected:
	std::shared_ptr<Graphic::Face> _defaultFont;
};

class ft_html_container_font : public virtual ft_html_container_base
{
public:


	uint_ptr create_font(const tchar_t* faceName, int size, int weight, font_style italic, unsigned int decoration, litehtml::font_metrics* fm) override
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
		//cairo_font_face_t* fnt = cairo_toy_font_face_create(, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        auto font = _defaultFont;
#ifdef WIPFONT
		auto fnt = CairoFontManager::get().getFont(faceName, size, weight, italic == fontStyleItalic);


		fm->ascent = fnt->ext().ascent;
		fm->descent = fnt->ext().descent;
		fm->height = fnt->ext().height;
		fm->x_height = fnt->x_ext().height;
		fm->draw_spaces = false;
#endif
        fm->ascent = font->ascender();
		fm->descent = font->descender();
		fm->height = font->height();
		fm->x_height = font->x_height();
		fm->draw_spaces = false;

		return (uint_ptr)0;
	}

	void delete_font(uint_ptr hFont) override
	{
		//cairo_font_face_destroy((cairo_font_face_t*)hFont);
	}

	int	text_width(const tchar_t* text, uint_ptr hFont) override
	{
        auto font = _defaultFont;
        return Graphic::FreetypeUtils::measureLine(font, text);
	}

	void draw_text(uint_ptr hdc, const tchar_t* text, uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override
	{
        FT_Vector     pen = { pos.x, pos.y };
        auto font = _defaultFont;
        Graphic::SurfaceRGBA &surface = *((Graphic::SurfaceRGBA*)hdc);

        font->draw_text( text, pen, [&](int x, int y, uint8_t p) 
	    {
            auto& out = surface.at( x, y );
            out.r = 255;
            out.g = 255;
            out.b = 255;
            out.a = p;
        } );
	}

	int	pt_to_px(int pt) override
	{
		return pt;
	}

	int	get_default_font_size() const override
	{
		if (_defaultFont)
			return _defaultFont->height();
		return 16;
	}
	const tchar_t* get_default_font_name() const override
	{
		return L"Arial";
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

    void get_language( litehtml::tstring& language, litehtml::tstring & culture ) const override
    {
    }
};

class ft_html_container : public ft_html_container_font, public ft_html_container_image, public ft_html_container_other
{
public:



};

Graphic::TextureImage::pointer HTMLRendererCairo::DrawOnBitmap(const std::wstring &str, int width, const Graphic::Font::pointer& defaultFont)
{
#if 0
	ft_html_container painter;

	if (defaultFont)
		painter.SetDefaultFont(defaultFont);

	litehtml::context ctx;
	ctx.load_master_stylesheet(HTMLUtils::mxmaster_css().c_str());


	auto document = document::createFromString(str.c_str(), &painter, &ctx);
	document->render(width);




	cairo_surface_t *surface;
	cairo_t *cr;

	int w = document->width(), h = document->height();
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
	cr = cairo_create(surface);

	
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
 	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

	//cairo_set_source_rgb(cr, 1., 1., 1.);
	//cairo_paint_with_alpha(cr, 1.0);


	auto clip = litehtml::position(0, 0, w, h);
	document->draw((uint_ptr)cr, 0, 0, &clip);


	//cairo_surface_write_to_png(surface, "d:/cairof.png");

	auto data = cairo_image_surface_get_data(surface);
	for (int i = 0; i < w*h * 4; i += 4)
	{
		std::swap(data[i], data[i + 2]);

		unsigned char r = data[i];
		unsigned char g = data[i+1];
		unsigned char b = data[i+2];
		unsigned char a = data[i+3];
	}

	auto bitmap = Graphic::Surface::Create(data, GL_RGBA, w, h);
	//bitmap->save("d:/cairof2.png");

	cairo_surface_destroy(surface);
	cairo_destroy(cr);
	return bitmap;
#endif
    auto ftFace = std::make_shared<Graphic::Face>(Paths::get().pathToResource("font/arial.ttf"), 16);

    ft_html_container painter;
	if (ftFace)
		painter.SetDefaultFont(ftFace);

	litehtml::context ctx;
	ctx.load_master_stylesheet(HTMLUtils::mxmaster_css().c_str());


	auto document = document::createFromString(str.c_str(), &painter, &ctx);
	document->render(width);

    int w = document->width(), h = document->height();
#ifdef __EMSCRIPTEN__
	w = gl::UpperPowerOfTwo(w);
	w = gl::UpperPowerOfTwo(w);
#endif    
    
    Graphic::SurfaceRGBA surface(w, h);

    auto clip = litehtml::position(0, 0, w, h);
    document->draw((uint_ptr)&surface, 0, 0, &clip);

    return Graphic::TextureImage::Create(surface);
}
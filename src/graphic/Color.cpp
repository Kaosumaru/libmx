#include "Color.h"
using namespace MX;


Color::Color()
{
	Set(1.0f,1.0f,1.0f,1.0f);
}

Color::Color(const unsigned &argb_color)
{
	unsigned char a = (argb_color & 0xFF000000) >> 24;
	unsigned char r = (argb_color & 0x00FF0000) >> 16;
	unsigned char g = (argb_color & 0x0000FF00) >> 8;
	unsigned char b = argb_color & 0x000000FF;
	Set(r/255.0f,g/255.0f,b/255.0f,a/255.0f);
}
Color::Color(unsigned char r, unsigned char g, unsigned char b)
{
	Set(r/255.0f,g/255.0f,b/255.0f,1.0f);
}
Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	Set(r/255.0f,g/255.0f,b/255.0f,a/255.0f);
}

Color::Color(float r, float g, float b)
{
	Set(r,g,b,1.0f);
}
Color::Color(float r, float g, float b, float a)
{
	Set(r,g,b,a);
}

void Color::Set(float r, float g, float b, float a)
{
	_r = r;
	_g = g;
	_b = b;
	_a = a;
}

void Color::SetA(float a)
{
	_a = a;
}

Color Color::lerp(const Color& color1, const Color& color2, float p)
{
	float a = color2.a() * p + color1.a() * (1.0f - p);
	float r = color2.r() * p + color1.r() * (1.0f - p);
	float g = color2.g() * p + color1.g() * (1.0f - p);
	float b = color2.b() * p + color1.b() * (1.0f - p);
	return Color(r,g,b,a);
}

float Color::a() const
{
	return _a;
}
float Color::r() const
{
	return _r;
}
float Color::g() const
{
	return _g;
}
float Color::b() const
{
	return _b;
}


Color Color::operator + (const Color& color) const
{
	return Color(r() + color.r(), g() + color.g(), b() + color.b(), a() + color.a());
}
Color Color::operator - (const Color& color) const
{
	return Color(r() - color.r(), g() - color.g(), b() - color.b(), a() - color.a());
}
Color Color::operator * (const Color& color) const
{
	return Color(r() * color.r(), g() * color.g(), b() * color.b(), a() * color.a());
}

Color Color::operator * (float v) const
{
	return Color(r() * v, g() * v, b() * v, a() * v);
}

bool Color::operator == (const Color& color) const
{
	return _color == color._color;
}
bool Color::operator != (const Color& color) const
{
	return _color != color._color;
}

unsigned int Color::toIntRGBA() const
{
	unsigned int ret = 0;
	unsigned char *c = (unsigned char*)&ret;
	c[3] = (unsigned char)(r() * 255.0f);
	c[2] = (unsigned char)(g() * 255.0f);
	c[1] = (unsigned char)(b() * 255.0f);
	c[0] = (unsigned char)(a() * 255.0f);
	return ret;
}

unsigned int Color::toIntARGB() const
{
	unsigned int ret = 0;
	unsigned char *c = (unsigned char*)&ret;
	c[3] = (unsigned char)(a() * 255.0f);
	c[2] = (unsigned char)(r() * 255.0f);
	c[1] = (unsigned char)(g() * 255.0f);
	c[0] = (unsigned char)(b() * 255.0f);
	return ret;
}

ColorExtended::ColorExtended()
{
	
}

ColorExtended::ColorExtended(const unsigned &argb_color) : _original(argb_color), _current(_original)
{

}
ColorExtended::ColorExtended(unsigned char r, unsigned char g, unsigned char b) : _original(r, g, b), _current(_original)
{
	
}
ColorExtended::ColorExtended(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : _original(r, g, b, a), _current(_original)
{
	
}

ColorExtended::ColorExtended(float r, float g, float b) : _original(r, g, b), _current(_original)
{
	
}
ColorExtended::ColorExtended(float r, float g, float b, float a) : _original(r, g, b, a), _current(_original)
{
	
}

ColorExtended& ColorExtended::operator = (const Color& new_color)
{
	_current = new_color;
	_original = _current;
	return *this;
}

ColorExtended& ColorExtended::operator = (const unsigned& new_color)
{
	_current = new_color;
	_original = _current;
	return *this;
}

void ColorExtended::Set(float r, float g, float b, float a)
{
	_current.Set(r, g, b, a);
	_original = _current;
}

void ColorExtended::SetA(float a)
{
	_current.SetA(a);
	_original = _current;
}

float ColorExtended::a() const
{
	return _current.a();
}
float ColorExtended::r() const
{
	return _current.r();
}
float ColorExtended::g() const
{
	return _current.g();
}
float ColorExtended::b() const
{
	return _current.b();
}


void ColorExtended::SetCurrent(const Color& current)
{
	_current = current;
}

Color& ColorExtended::original()
{
	return _original;
}
Color& ColorExtended::current()
{
	return _current;
}
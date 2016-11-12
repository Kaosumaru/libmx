#pragma once

namespace MX{

class Color
{
public:
	Color();
	Color(const unsigned &color);
	Color(unsigned char r, unsigned char g, unsigned char b);
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	Color(float r, float g, float b);
	Color(float r, float g, float b, float a);


	Color operator + (const Color& color) const;
	Color operator - (const Color& color) const;
	Color operator * (const Color& color) const;
	Color operator * (float v) const;

	bool operator == (const Color& color) const;
	bool operator != (const Color& color) const;

	static Color lerp(const Color& color1, const Color& color2, float p); 

	static Color white() { return { 1.0f,1.0f,1.0f,1.0f }; }
	static Color black() { return{ .0f,.0f,.0f,1.0f }; }

	void Set(float r, float g, float b, float a = 1.0f);

	void SetA(float a = 1.0f);

	float a() const;
	float r() const;
	float g() const;
	float b() const;

	float& operator[](int n)
	{
		return _color[n];
	}


	unsigned int toIntRGBA() const;
	unsigned int toIntARGB() const;
protected:
	union
	{
		float _color[4];
		struct
		{
			float _r,_g,_b,_a;
		};
	};
	
};



class ColorExtended
{
public:
	ColorExtended();
	ColorExtended(const unsigned &color);
	ColorExtended(unsigned char r, unsigned char g, unsigned char b);
	ColorExtended(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	ColorExtended(float r, float g, float b);
	ColorExtended(float r, float g, float b, float a);


	ColorExtended& operator = (const Color& new_color);
	ColorExtended& operator = (const unsigned& new_color);

	Color& original();
	Color& current();

	operator const Color&() const { return _current;  }

	void Set(float r, float g, float b, float a = 1.0f);

	void SetA(float a = 1.0f);

	float a() const;
	float r() const;
	float g() const;
	float b() const;

	void SetCurrent(const Color& current);

protected:
	Color _original;
	Color _current;

};

}

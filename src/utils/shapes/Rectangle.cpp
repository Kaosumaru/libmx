#include "Rectangle.h"
#include "utils/Random.h"
#include <algorithm>

using namespace MX;




Rectangle::Rectangle(const glm::vec2 &v1, const glm::vec2 &v2) : x1(v1.x), y1(v1.y), x2(v2.x), y2(v2.y)
{

}

Rectangle::Rectangle(float x1, float y1,float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2)
{
}

MX::Rectangle Rectangle::fromWH(float x, float y, float width, float height)
{
	return Rectangle(x, y, x + width, y + height);
}

void Rectangle::Expand(float value)
{
	x1 -= value;
	x2 += value;

	y1 -= value;
	y2 += value;
}
	
void Rectangle::SetPointWH(float x, float y, float w, float h)
{
	x1 = x;
	y1 = y;
	SetWidth(w);
	SetHeight(h);
}


void Rectangle::NumLayoutIn(const Rectangle& container, int n, bool round)
{
	static std::pair<int, int> arr[] = 
	{ 
		{0,0},
		{-1,1},{0,1},{ 1,1 },
		{ -1,0 },{ 0,0 },{ 1,0 },
		{ -1,-1 },{ 0,-1 },{ 1,-1 }
	};

	auto &p = arr[n];
	float pointX, pointY;

    if (p.first == 0)
    {
        pointX = container.x1 + (container.width() - width()) / 2.0f;
        if (round)
            pointX = std::round(pointX);
    }
	else if (p.first == -1)
		pointX = container.x1;
	else 
		pointX = container.x2 - width();

    if (p.second == 0)
    {
        pointY = container.y1 + (container.height() - height()) / 2.0f;
        if (round)
            pointY = std::round(pointY);
    }
	else if (p.second == -1)
		pointY = container.y1;
	else
		pointY = container.y2 - height();

	MoveTo(pointX, pointY);
}

void Rectangle::CenterIn(const Rectangle& container)
{
	float pointX = container.x1 + (container.width() - width()) / 2.0f;
	float pointY = container.y1 + (container.height() - height()) / 2.0f;

	MoveTo(pointX, pointY);
}

void Rectangle::SetWidth(float value)
{
	x2 = x1 + value;
}

void Rectangle::SetHeight(float value)
{
	y2 = y1 + value;
}

void Rectangle::SetPoint(float x, float y)
{
	x1 = x;
	x2 = x1 + 1.0f;

	y1 = y;
	y2 = y1 + 1.0f;

}

float& Rectangle::point(int i)
{
	switch (i)
	{
	case 0:
		return x1;
	case 1:
		return y1;
	case 2:
		return x2;
	case 3:
		return y2;
	}
	return x1;
}

bool Rectangle::intersects(const Rectangle& rectangle) const 
{
	return x1 < rectangle.x2 && x2 > rectangle.x1 && y1 < rectangle.y2 && y2 > rectangle.y1;
}

MX::Rectangle Rectangle::intersection(const Rectangle& other)
{
	if (!intersects(other))
		return MX::Rectangle();
	return MX::Rectangle(std::max(x1, other.x1), std::max(y1, other.y1), std::min(x2, other.x2), std::min(y2, other.y2));
}

bool Rectangle::contains(const Rectangle& rectangle) const
{
	return x1 <= rectangle.x1 && x2 >= rectangle.x2 && y1 <= rectangle.y1 && y2 >= rectangle.y2;
}

bool Rectangle::contains(const glm::vec2& point) const
{
	return x1 <= point.x && x2 > point.x && y1 <= point.y && y2 > point.y;
}

float Rectangle::width() const
{
	return x2 - x1;
}
float Rectangle::height() const
{
	return y2 - y1;
}

glm::vec2 Rectangle::center() const
{
	return glm::vec2(x1 + width() / 2.0f, y1 + height() / 2.0f);
}

glm::vec2 Rectangle::randomPointInside() const
{
	return glm::vec2(Random::randomRange(std::make_pair(x1,x2)), Random::randomRange(std::make_pair(y1,y2)));
}

bool Rectangle::empty() const
{
	return x1 == 0.0f && x2 == 0.0f && y1 == 0.0f && y2 == 0.0f;
}

void Rectangle::Draw()
{
#ifdef WIP
	auto renderer = Graphic::CinderRenderer::get().Use();
	ci::gl::color(0.0f,0.0f,1.0f);
	ci::gl::drawStrokedRect({ x1, y1, x2, y2});
#endif
}


std::pair < float, int >  Rectangle::closestEdge(const glm::vec2& point) const
{
	using Pair = std::pair < float, int >;
	auto m = std::min<Pair>({ { std::abs(point.x - x1), 3 },
	{ std::abs(point.x - x2), 1 },
	{ std::abs(point.y - y1), 0 },
	{ std::abs(point.y - y2), 2 }
	});

	return m;
}

glm::vec2 Rectangle::closestPoint(const glm::vec2& point) const
{
	switch (closestEdge(point).second)
	{
	case 3:
		return {x1, point.y};
	case 1:
		return{ x2, point.y };
	case 0:
		return{ point.x, y1 };
	default:
	case 2:
		return{ point.x, y2 };
	}
}

Margins::Margins(float left, float right,float top, float bottom) : left(left), right(right), top(top), bottom(bottom)
{
}

float Margins::vMargins() const
{
	return top + bottom;
}
float Margins::hMargins() const
{
	return left + right;
}

#include "Line.h"
#include "Quad.h"
#include "utils/Random.h"
#include <list>

using namespace MX;



Line::Line(float x1, float y1,float x2, float y2) : point1(x1,y1), point2(x2,y2)
{
}

Line::Line(const glm::vec2& p1, const glm::vec2& p2) : point1(p1), point2(p2)
{

}


void Line::Draw(const MX::Color& color, float width)
{
#ifdef WIP
	auto renderer = Graphic::CinderRenderer::get().Use();
	ci::gl::color(color.toColorA());
	ci::gl::lineWidth(width);
	ci::gl::drawLine({ point1.x, point1.y }, { point2.x, point2.y });
#endif
}


glm::vec2 Line::pointBetween(float ratio) const
{
	return glm::vec2(point1.x + (point2.x-point1.x)*ratio, point1.y + (point2.y-point1.y)*ratio);
}

glm::vec2 Line::center() const
{
	return pointBetween(0.5f);
}

Line Line::perpendicular(float ratio, float length) const
{
	glm::vec2 point = pointBetween(ratio);

	if (point1.x == point2.x)
	{
		glm::vec2 p1(point.x - length/2.0f, point.y);
		glm::vec2 p2(point.x + length/2.0f, point.y);

		if (point1.y > point2.y)
			return Line(p1,p2);
		else
			return Line(p2, p1);
	}


	if (point1.y == point2.y)
	{
		glm::vec2 p1(point.x, point.y - length / 2.0f);
		glm::vec2 p2(point.x, point.y + length / 2.0f);
		if (point1.x < point2.x)
			return Line(p1, p2);
		else
			return Line(p2, p1);
	}
	
	float inverse_slope = -1.0f/slope();

	float b = point.y - inverse_slope*point.x;

	float length_per_x = sqrt(1.0f+inverse_slope*inverse_slope);
	float delta = (length/2.0f)/length_per_x;

	float tx = point.x - delta;
	glm::vec2 p1(tx, tx*inverse_slope + b);

	tx = point.x + delta;
	glm::vec2 p2(tx, tx*inverse_slope + b);

	//return Line(p2,p1);

	bool choice = true;

	if (slope() > 0)
		choice = !choice;

	if (point1.x > point2.x)
		choice = !choice;

	if (choice)
		return Line(p1,p2);
	
	return Line(p2,p1);
}

float Line::slope() const
{
	return (point2.y-point1.y)/(point2.x-point1.x);
}

float Line::length() const
{
	return distanceBetween(point1, point2);
}

float Line::lengthSquared() const
{
	return distanceBetweenSquared(point1, point2);
}


float Line::angle1() const
{
	return angle(point2-point1);
}

Line Line::lineByRotation1(float angle, float length_ratio)
{
	auto a = angle1() + angle;
	Line ret;
	ret.point1 = point1;
	auto direction = MX::CreateVectorFromAngle(a);
	ret.point2 = point1 + direction*length()*length_ratio;
	return ret;
}


glm::vec2 Line::lineVector() const
{
	return point2 - point1;
}

void Line::Shrink(float percent)
{
	auto p1 = pointBetween(percent/2.f);
	auto p2 = pointBetween(1.0 - percent/2.f);
	point1 = p1;
	point2 = p2;
}

void Line::Move(float x, float y)
{
	point1.x += x;
	point1.y += y;
	point2.x += x;
	point2.y += y;
}

void Line::Move(const glm::vec2& p)
{
	point1 += p;
	point2 += p;
}




Line Line::mean(const Line& line1, const Line& line2)
{
	return Line(MX::mean(line1.point1, line2.point2), MX::mean(line1.point2, line2.point2));
}



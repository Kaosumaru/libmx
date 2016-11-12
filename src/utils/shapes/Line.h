#ifndef MXLINE
#define MXLINE
#include "Shapes.h"

namespace cinder {
	class Line;
}


namespace MX
{


struct Line
{
	glm::vec2 point1;
	glm::vec2 point2;

	explicit Line(float x1=0, float y1=0,float x2=0, float y2=0);
	explicit Line(const glm::vec2& p1, const glm::vec2& p2);
	

	void Draw(const MX::Color& color = MX::Color(), float width = 1.0f);


	glm::vec2 pointBetween(float ratio) const;
	glm::vec2 center() const;
	glm::vec2 lineVector() const;

	Line perpendicular(float ratio, float length) const;

	float slope() const;
	float length() const;
	float lengthSquared() const;

	float angle1() const;
	Line lineByRotation1(float angle, float length_ratio = 1.0f);

	void Shrink(float percent);
	void Move(float x, float y);
	void Move(const glm::vec2& p);

	static Line mean(const Line& line1, const Line& line2);
};






}

#endif

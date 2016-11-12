#include "Ellipse.h"
#include <cmath>

using namespace MX;



void Ellipse::Draw(const MX::Color& color)
{
#ifdef WIP
	auto renderer = Graphic::CinderRenderer::get().Use();
	ci::gl::color(color.toColorA());
	ci::gl::drawStrokedEllipse({center.x, center.y}, rX, rY);
#endif
}


bool Ellipse::isInside(const glm::vec2& point)
{
#if 0
	float radius = std::max(rX, rY);
	if (distanceBetweenSquared(point, center) > (radius * radius))
		return false;
#endif
	auto dist = pow((point.x - center.x), 2) / (rX*rX) + pow((point.y - center.y), 2) / (rY*rY);
	return dist <= 1.0;
}

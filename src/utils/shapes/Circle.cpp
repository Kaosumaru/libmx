#include "Circle.h"
#include "Utils/Random.h"
#include <cmath>

using namespace MX;

glm::vec2 Circle::RandomPointInside()
{
	while (true)
	{
		float x = (Random::randomRange(std::make_pair(-(radius), radius)));
		float y = (Random::randomRange(std::make_pair(-(radius), radius)));
		if (sqrt((x * x) + (y * y)) <= radius)
			return{ x, y };
	}

}

glm::vec2 Circle::RandomPointInsideMoreAtCenter()
{
	return center + MX::CreateVectorFromAngle(Random::randomRange((float)MX_2PI)) * Random::randomRange(radius);
}

glm::vec2 Circle::RandomPointCircumference()
{
	return center + MX::CreateVectorFromAngle(Random::randomRange((float)MX_2PI)) * radius;
}
glm::vec2 Circle::RandomPointCircumference(std::pair<float, float> range)
{
	return center + MX::CreateVectorFromAngle(Random::randomRange(range) * (float)MX_2PI) * radius;
}

glm::vec2 Circle::PointFromAngle(float ang)
{
	return center + MX::CreateVectorFromAngle(ang) * radius;
}


void Circle::Draw(const MX::Color& color)
{
#ifdef WIP
	auto renderer = Graphic::CinderRenderer::get().Use();
	ci::gl::color(color.toColorA());
	ci::gl::drawStrokedCircle({center.x, center.y}, radius, 32);
#endif
}


bool Circle::isInside(const glm::vec2 &point)
{
	return distanceBetweenSquared(point, center) <= (radius * radius);
}
void Circle::Clip(glm::vec2 &point)
{
	if (isInside(point))
		return;
	auto v = MX::vectorFromPointToPoint(center, point);
	normalize(v);
	point = center + v * radius;
}

#ifndef MXELLIPSE
#define MXELLIPSE
#include<memory>
#include "glm/vec2.hpp"
#include "Graphic/Color.h"
#include <cmath>
#include <time.h>
namespace MX
{

struct Ellipse
{
	Ellipse() {}
	Ellipse(float rX, float rY) : rX(rX), rY(rY) {}
	Ellipse(const glm::vec2& center, float rX, float rY) : center(center), rX(rX), rY(rY) {}

	glm::vec2 center = { 0, 0 };
	float rX = 0.0f, rY = 0.0f;

	bool isInside(const glm::vec2 &point);

	void Draw(const MX::Color& color = MX::Color());
};


}

#endif

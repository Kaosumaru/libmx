#pragma once
#include "Shapes.h"
#include <cmath>
#include <memory>
#include <time.h>

namespace MX
{

struct Circle : public Geometry
{
    Circle() { }
    Circle(float radius)
        : radius(radius)
    {
    }
    Circle(const glm::vec2& center, float radius)
        : center(center)
        , radius(radius)
    {
    }

    glm::vec2 center = { 0, 0 };
    float radius = 0.0f;

    glm::vec2 RandomPointInside();
    glm::vec2 RandomPointInsideMoreAtCenter();
    glm::vec2 RandomPointCircumference();
    glm::vec2 RandomPointCircumference(std::pair<float, float> range);
    glm::vec2 PointFromAngle(float ang);

    bool isInside(const glm::vec2& point);
    void Clip(glm::vec2& point);

    void Draw(const MX::Color& color = MX::Color());
};

}
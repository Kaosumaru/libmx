#include "OrientedRectangle.h"
#include "utils/Vector2.h"

using namespace MX;

OrientedRectangle::OrientedRectangle()
{
}

OrientedRectangle::OrientedRectangle(const glm::vec2& center, const float w, const float h, float angle)
{
    glm::vec2 X(cos(angle), sin(angle));
    glm::vec2 Y(-sin(angle), cos(angle));

    X *= w / 2.f;
    Y *= h / 2.f;

    _corners.point[0] = center - X - Y;
    _corners.point[1] = center + X - Y;
    _corners.point[2] = center - X + Y;
    _corners.point[3] = center + X + Y;

    computeAxes();
}

OrientedRectangle::OrientedRectangle(const Line& centerLine, float width)
{
    _corners = MX::Quad(centerLine, width);
    computeAxes();
}

OrientedRectangle::OrientedRectangle(const MX::Rectangle& rectangle)
{
    _corners.point[0] = rectangle.pointTopLeft();
    _corners.point[1] = rectangle.pointTopRight();
    _corners.point[2] = rectangle.pointBottomLeft();
    _corners.point[3] = rectangle.pointBottomRight();
    computeAxes();
}

bool OrientedRectangle::overlaps1Way(const OrientedRectangle& other) const
{
    for (int a = 0; a < 2; ++a)
    {

        double t = VectorDotProduct(other.corner(0), _axis[a]);

        // Find the extent of box 2 on axis a
        double tMin = t;
        double tMax = t;

        for (int c = 1; c < 4; ++c)
        {
            t = VectorDotProduct(other.corner(c), _axis[a]);

            if (t < tMin)
            {
                tMin = t;
            }
            else if (t > tMax)
            {
                tMax = t;
            }
        }

        // We have to subtract off the origin

        // See if [tMin, tMax] intersects [0, 1]
        if ((tMin > 1 + _origin[a]) || (tMax < _origin[a]))
        {
            // There was no intersection along this dimension;
            // the boxes cannot possibly overlap.
            return false;
        }
    }

    // There was no dimension along which there is no intersection.
    // Therefore the boxes overlap.
    return true;
}

/** Updates the axes after the corners move.  Assumes the
corners actually form a rectangle. */
void OrientedRectangle::computeAxes()
{
    _axis[0] = corner(1) - corner(0);
    _axis[1] = corner(2) - corner(0);

    _angle = MX::angle(_axis[0]);
    _width = (float)_axis[0].length();
    _height = (float)_axis[1].length();

    // Make the length of each axis 1/edge length so we know any
    // dot product must be less than 1 to fall within the edge.

    for (int a = 0; a < 2; ++a)
    {
        _axis[a] /= lengthSquared(_axis[a]);
        _origin[a] = VectorDotProduct(corner(0), _axis[a]);
    }
}

bool OrientedRectangle::overlaps(const OrientedRectangle& other) const
{
    return overlaps1Way(other) && other.overlaps1Way(*this);
}

void OrientedRectangle::Shift(float x, float y)
{
    _corners.Shift(x, y);
    computeAxes();
}

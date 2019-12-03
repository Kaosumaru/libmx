#ifndef MXORIENTEDRECTANGLE
#define MXORIENTEDRECTANGLE
#include "Shapes.h"
#include "utils/shapes/Line.h"
#include "utils/shapes/Quad.h"
#include "utils/shapes/Rectangle.h"

namespace cinder
{
class Line;
}

namespace MX
{
struct Line;

struct OrientedRectangle : public Geometry
{
    OrientedRectangle();
    OrientedRectangle(const glm::vec2& center, const float w, const float h, float angle);
    OrientedRectangle(const Line& centerLine, float width);
    OrientedRectangle(const Rectangle& rectangle);

    void Draw() const { _corners.Draw(); }
    void Shift(float x, float y);

    Rectangle bounds() const { return _corners.bounds(); }
    bool overlaps(const OrientedRectangle& other) const;

    const glm::vec2& corner(int i) const { return _corners.point[i]; }

    glm::vec2 center() const { return (_corners.point[0] + _corners.point[3]) * 0.5f; }
    float angle() const { return _angle; }
    float width() const { return _width; }
    float height() const { return _height; }

protected:
    bool overlaps1Way(const OrientedRectangle& other) const;

    /** Updates the axes after the corners move.  Assumes the
	corners actually form a rectangle. */
    void computeAxes();

    Quad _corners;

    /** Two edges of the box extended away from corner[0]. */
    glm::vec2 _axis[2];

    /** origin[a] = corner[0].dot(axis[a]); */
    double _origin[2];
    float _angle = 0.0f;
    float _width = 0.0f;
    float _height = 0.0f;
};

}

#endif

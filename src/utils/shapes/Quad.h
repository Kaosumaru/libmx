#ifndef MXQUAD
#define MXQUAD
#include "Line.h"
#include "Rectangle.h"
#include "Shapes.h"

namespace MX
{
struct Line;

struct Quad
{
    Quad();
    Quad(const Line& line, float width);
    Quad(const glm::vec2& top_left, const glm::vec2& top_right, const glm::vec2& bottom_right, const glm::vec2& bottom_left);
    Quad(const Line& line1, const Line& line2);
    Quad(const Rectangle& rect);
    glm::vec2 point[4];

    void Draw() const;
    bool zero() const;

    //lodo optimize
    glm::vec2 center() const { return bounds().center(); }

    void Shift(float x, float y);

    Rectangle bounds() const;

    Line top_line() const { return MX::Line(point[0], point[1]); }
    Line bottom_line() const { return MX::Line(point[2], point[3]); }
};

}

#endif

#include "Quad.h"
#include "utils/Random.h"
#include <algorithm>

using namespace MX;

MX::Quad::Quad()
{
}
MX::Quad::Quad(const glm::vec2& top_left, const glm::vec2& top_right, const glm::vec2& bottom_right, const glm::vec2& bottom_left)
{
    point[0] = top_left;
    point[1] = top_right;
    point[2] = bottom_left;
    point[3] = bottom_right;
}

MX::Quad::Quad(const Line& top_line, const Line& bottom_line)
{
    point[0] = top_line.point1;
    point[1] = top_line.point2;
    point[2] = bottom_line.point1;
    point[3] = bottom_line.point2;
}

MX::Quad::Quad(const MX::Rectangle& rect)
{
    Rectangle r = rect;
    r.x2 += 1.0f;
    r.y2 += 1.0f;

    point[0] = r.pointTopLeft();
    point[1] = r.pointTopRight();
    point[2] = r.pointBottomLeft();
    point[3] = r.pointBottomRight();
}

MX::Quad::Quad(const Line& line, float width)
{
    auto bottom = line.perpendicular(0.0f, width);
    auto top = bottom;
    top.Move(line.lineVector());

    point[0] = top.point2;
    point[1] = top.point1;
    point[2] = bottom.point2;
    point[3] = bottom.point1;
}

void MX::Quad::Draw() const
{
#ifdef WIP
    auto renderer = Graphic::CinderRenderer::get().Use();
    //top
    Line(point[0], point[1]).Draw(MX::Color(1.0f, 1.0f, 0.0f));

    //right
    Line(point[1], point[3]).Draw(MX::Color(1.0f, 0.0f, 1.0f));

    //left
    Line(point[2], point[0]).Draw(MX::Color(1.0f, 0.0f, 0.0f));
    //down
    Line(point[2], point[3]).Draw(MX::Color(0.0f, 1.0f, 0.0f));
#endif
}

bool MX::Quad::zero() const
{
    for (unsigned i = 0; i < 4; i++)
        if (!MX::zero(point[i]))
            return false;
    return true;
}

void MX::Quad::Shift(float x, float y)
{
    for (auto& p : point)
        p += glm::vec2(x, y);
}

MX::Rectangle MX::Quad::bounds() const
{
    auto pair_x = std::minmax({ point[0].x, point[1].x, point[2].x, point[3].x });
    auto pair_y = std::minmax({ point[0].y, point[1].y, point[2].y, point[3].y });

    return MX::Rectangle(pair_x.first, pair_y.first, pair_x.second, pair_y.second);
}
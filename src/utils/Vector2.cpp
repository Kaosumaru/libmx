#include "Vector2.h"
#include "utils/shapes/Rectangle.h"

namespace MX
{
void clampTo(glm::vec2& v, const MX::Rectangle& r)
{
    v.x = std::max(v.x, r.x1);
    v.x = std::min(v.x, r.x2);
    v.y = std::max(v.y, r.y1);
    v.y = std::min(v.y, r.y2);
}
}

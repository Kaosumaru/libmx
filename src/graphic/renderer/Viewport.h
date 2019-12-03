#pragma once
#include "glm/glm.hpp"
#include "graphic/OpenGL.h"
#include "utils/Singleton.h"
#include "utils/shapes/Rectangle.h"
#include <vector>

namespace MX
{
class Viewport : public Singleton<Viewport>
{
public:
    static void Set(const Rectangle& m)
    {
        get()._stack.back() = m;
        UseCurrent();
    }

    static const Rectangle& current()
    {
        return get()._stack.back();
    }

    static void Push();
    static void Push(int w, int h);
    static void Pop();

protected:
    static void UseCurrent()
    {
        glViewport(0, 0, (int)current().width(), (int)current().height());
    }

    std::vector<Rectangle> _stack { 1 };
};
}

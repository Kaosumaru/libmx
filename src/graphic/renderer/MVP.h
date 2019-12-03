#pragma once
#include "glm/glm.hpp"
#include "graphic/OpenGL.h"
#include "utils/Singleton.h"
#include <vector>

namespace MX
{
class MVP : public Singleton<MVP>
{
public:
    static void SetProjection(const glm::mat4& m)
    {
        get().current()._projection = m;
        get().current().UpdateMVP();
    }

    static void translate(const glm::vec2& p);
    static void rotateZoom(const glm::vec2& center, const glm::vec2& scale, float angle);

    static const auto& mvp()
    {
        return get().current()._mvp;
    }

    static void Push();
    static void Pop();

protected:
    struct Data
    {
        glm::mat4 _projection;
        glm::mat4 _view;
        glm::mat4 _model;
        glm::mat4 _mvp;

        void UpdateMVP()
        {
            _mvp = _projection * _view * _model;
        }
    };

    Data& current() { return _stack.back(); }

    std::vector<Data> _stack { 1 };
};
}

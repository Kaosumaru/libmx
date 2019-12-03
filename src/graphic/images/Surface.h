#pragma once
#include "glm/vec4.hpp"
#include "utils/Array2D.h"

namespace MX
{
namespace Graphic
{
    using PixelRGBA = glm::tvec4<uint8_t>;

    template <typename T>
    class Surface : public Array2D<T>
    {
    public:
        using Array2D<T>::Array2D;

        void Clear(const T& t)
        {
            for (auto& p : *this)
                p = t;
        }

    protected:
    };

    using SurfaceRGBA = Surface<PixelRGBA>;
    using SurfaceGrayscale = Surface<uint8_t>;
}
}
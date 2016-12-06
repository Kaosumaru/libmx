#pragma once
#include "utils/Array2D.h"
#include "glm/vec4.hpp"

namespace MX
{
namespace Graphic
{
    using PixelRGBA = glm::tvec4<unsigned char>;

    template<typename T>
    class Surface : public Array2D<T>
    {
    public:
        using Array2D<T>::Array2D;

        void Clear( const T& t )
        {
            for ( auto& p : *this )
                p = t;
        }
    protected:

    };

    using SurfaceRGBA = Surface<PixelRGBA>;

}
}
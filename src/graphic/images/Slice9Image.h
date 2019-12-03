#pragma once
#include "Image.h"
#include "TextureImage.h"
#include <vector>

namespace MX
{
namespace Graphic
{

    class QuadImage : virtual public disable_copy_constructors
    {
    public:
        virtual void DrawArea(const MX::Rectangle& destination, const Color& color = Color()) {};
    };

    typedef std::shared_ptr<QuadImage> QuadImagePtr;

    class QuadImageHolder : public QuadImage
    {
    public:
        QuadImageHolder(const ImagePtr& image);
        void DrawArea(const MX::Rectangle& destination, const Color& color = Color());

    protected:
        ImagePtr _image;
    };

    class Slice9Image : public QuadImageHolder
    {
    public:
        Slice9Image(const MX::Margins& margins, const Graphic::TextureImage::pointer& image);
        Slice9Image(const std::vector<ImagePtr>& images);
        void DrawArea(const MX::Rectangle& destination, const Color& color = Color());

    protected:
        bool _tiled = false;
        MX::Margins _margins;
        ImagePtr _subImages[9];
    };

}
}

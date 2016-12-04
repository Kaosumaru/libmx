#include "Slice9Image.h"



using namespace MX;
using namespace MX::Graphic;


QuadImageHolder::QuadImageHolder(const ImagePtr& image)
{
	_image = image;
}
	
void QuadImageHolder::DrawArea(const MX::Rectangle &destination, const Color &color)
{
	_image->DrawArea(destination, Rectangle(0.0f, 0.0f, _image->Width(), _image->Height()), color);
}

Slice9Image::Slice9Image(const MX::Margins &margins, const Graphic::TextureImage::pointer& image) : QuadImageHolder(image)
{
	_margins = margins;

	_subImages[0] = TextureImage::Create(image, 0, 0, margins.left, margins.top);
	_subImages[1] = TextureImage::Create(image, margins.left, 0, image->Width() - margins.left - margins.right, margins.top);
	_subImages[2] = TextureImage::Create(image, image->Width() - margins.right, 0, margins.right, margins.top);

	_subImages[3] = TextureImage::Create(image, 0, margins.top, margins.left, image->Height() - margins.top - margins.bottom);
	_subImages[4] = TextureImage::Create(image, margins.left, margins.top, image->Width() - margins.left - margins.right, image->Height() - margins.top - margins.bottom);
	_subImages[5] = TextureImage::Create(image, image->Width() - margins.right, margins.top, margins.right, image->Height() - margins.top - margins.bottom);

	_subImages[6] = TextureImage::Create(image, 0, image->Height() - margins.bottom, margins.left, margins.bottom);
	_subImages[7] = TextureImage::Create(image, margins.left, image->Height() - margins.bottom, image->Width() - margins.left - margins.right, margins.bottom);
	_subImages[8] = TextureImage::Create(image, image->Width() - margins.right, image->Height() - margins.bottom, margins.right, margins.bottom);

}

Slice9Image::Slice9Image(const std::vector<ImagePtr>& images) : QuadImageHolder(nullptr)
{
	_tiled = true;
	if (images.size() < 9)
		return;

	_margins.left = images[0]->Width();
	_margins.right = images[2]->Width();

	_margins.top = images[0]->Height();
	_margins.bottom = images[6]->Height();

	for (int i = 0; i < 9; i++)
		_subImages[i] = images[i];
}


void Slice9Image::DrawArea(const MX::Rectangle &destination, const Color &color)
{
    using FunctionType = void (Image::*)(const MX::Rectangle &, const Color &);
    FunctionType drawCenter = _tiled ? &Image::DrawTiled : static_cast<FunctionType>(&Image::DrawArea);

    auto draw = [&](int n, const MX::Rectangle &destination)
    {
        auto img = _subImages[n].get();
        img->DrawArea(destination, color);
    };

    auto drawC = [&](int n, const MX::Rectangle &destination)
    {
        auto img = _subImages[n].get();
        (img->*drawCenter)(destination, color);
    };

    int xdelta[] = { -1, 0 ,1 };
    int ydelta[] = { 1, 4 , 7 };

    if (Image::Settings::flipX)
    {
        std::swap(xdelta[0], xdelta[2]);
        std::swap(_margins.left, _margins.right);
    }

    if (Image::Settings::flipY)
    {
        std::swap(ydelta[0], ydelta[2]);
        std::swap(_margins.top, _margins.bottom);
    }

    draw (ydelta[0] + xdelta[0], Rectangle(destination.x1, destination.y1, destination.x1 + _margins.left, destination.y1 + _margins.top));
    drawC(ydelta[0] + xdelta[1], Rectangle(destination.x1 + _margins.left, destination.y1, destination.x2 - _margins.right, destination.y1 + _margins.top));
    draw (ydelta[0] + xdelta[2], Rectangle(destination.x2 - _margins.right, destination.y1, destination.x2, destination.y1 + _margins.top));


    drawC(ydelta[1] + xdelta[0], Rectangle(destination.x1, destination.y1 + _margins.top, destination.x1 + _margins.left, destination.y2 - _margins.bottom));
    drawC(ydelta[1] + xdelta[1], Rectangle(destination.x1 + _margins.left, destination.y1 + _margins.top, destination.x2 - _margins.right, destination.y2 - _margins.bottom));
    drawC(ydelta[1] + xdelta[2], Rectangle(destination.x2 - _margins.right, destination.y1 + _margins.top, destination.x2, destination.y2 - _margins.bottom));

    draw (ydelta[2] + xdelta[0], Rectangle(destination.x1, destination.y2 - _margins.bottom, destination.x1 + _margins.left, destination.y2));
    drawC(ydelta[2] + xdelta[1], Rectangle(destination.x1 + _margins.left, destination.y2 - _margins.bottom, destination.x2 - _margins.right, destination.y2));
    draw (ydelta[2] + xdelta[2], Rectangle(destination.x2 - _margins.right, destination.y2 - _margins.bottom, destination.x2, destination.y2));

    if (Image::Settings::flipX)
    {
        std::swap(_margins.left, _margins.right);
    }

    if (Image::Settings::flipY)
    {
        std::swap(_margins.top, _margins.bottom);
    }
}
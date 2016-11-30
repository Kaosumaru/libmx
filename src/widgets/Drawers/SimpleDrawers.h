#ifndef MXSIMPLEDRAWER
#define MXSIMPLEDRAWER
#include "Utils/Utils.h"
#include "Drawer.h"
#include "Graphic/Images/Image.h"
#include "Graphic/Images/Slice9Image.h"

namespace MX
{
namespace Widgets
{

class Widget;



class SimpleDrawer : public Drawer
{
public:
	SimpleDrawer(const Graphic::ImagePtr& image);
	void DrawBackground();
protected:
	Graphic::ImagePtr _image;
};

class SimpleQuadDrawer : public Drawer
{
public:
	SimpleQuadDrawer(const Graphic::QuadImagePtr& image);
	void DrawBackground();
protected:
	Graphic::QuadImagePtr _image;
};

class TestButtonDrawer : public Drawer
{
public:
	TestButtonDrawer(const Graphic::ImagePtr& image1, const Graphic::ImagePtr& image2, const MX::Graphic::ImagePtr& disabled);
	void DrawBackground();
	bool ShouldDrawWidget();
protected:
	Graphic::ImagePtr _image1;
	Graphic::ImagePtr _image2;
	Graphic::ImagePtr _disabled;
};

}
}

#endif

#include "SimpleDrawers.h"
#include "widgets/Widget.h"

using namespace MX::Widgets;



SimpleDrawer::SimpleDrawer(const MX::Graphic::ImagePtr& image)
{
	_image = image;
}

void SimpleDrawer::DrawBackground()
{

	float x = Destination::current().x(), y = Destination::current().y();

	if (Widget::current().geometry.color.a() != 0x0 && _image)
	{
		float w1 = Destination::current().width(), h1 = Destination::current().height();
		auto w2 = _image->Width(), h2 = _image->Height();

		_image->DrawCentered( {}, { x, y }, { w1 / w2, h1 / h2 }, Widget::current().geometry.angle, Widget::current().geometry.color );
	}
}


SimpleQuadDrawer::SimpleQuadDrawer(const MX::Graphic::QuadImagePtr& image)
{
	_image = image;
}
void SimpleQuadDrawer::DrawBackground()
{
	float x = Destination::current().x(), y = Destination::current().y();

	if (Widget::current().geometry.color.a() != 0x0 && _image)
	{
		_image->DrawArea(MX::Rectangle::fromWH(x, y, Destination::current().width(), Destination::current().height()), Widget::current().geometry.color);
	}
}



TestButtonDrawer::TestButtonDrawer(const MX::Graphic::ImagePtr& image1, const MX::Graphic::ImagePtr& image2, const MX::Graphic::ImagePtr& disabled)
{
	_image1 = image1;
	_image2 = image2;
	_disabled = disabled;
}

bool TestButtonDrawer::ShouldDrawWidget()
{
	if (isDragged() && !drawDragged())
		return false;
	return true;
}

void TestButtonDrawer::DrawBackground()
{
	if (isDragged() && !drawDragged())
		return;
	float x = Destination::current().x(), y = Destination::current().y();

	if (Widget::current().geometry.color.a() != 0x0 && _image1 && _image2)
	{
		auto &button = dynamic_cast<ButtonWidget&>(Widget::current());
		MX::Graphic::ImagePtr image;
		if (!button.enabled())
		{
			image = _disabled;
		}
		else
			image = button.pressed() || button.selected() ? _image2 : _image1;


		float w1 = Destination::current().width(), h1 = Destination::current().height();
		auto w2 = image->Width(), h2 = image->Height();

		image->DrawCentered( {}, { x, y }, { w1 / w2, h1 / h2 }, Widget::current().geometry.angle, Widget::current().geometry.color );
	}
}


#ifndef MXDRAWERS
#define MXDRAWERS
#include "Utils/MXUtils.h"
#include "MXDrawer.h"
#include "Graphic/Images/MXImage.h"
#include "Graphic/Images/MXSlice9Image.h"
#include "Script/MXScriptObject.h"
#include "Graphic/MXScriptableColor.h"
#include "Widgets/MXWidget.h"
#include "MXDrawerUtils.h"

namespace MX
{
namespace Widgets
{


    class ColoredDrawer : public MX::Widgets::Drawer
    {
    protected:
        ColoredDrawer() {}
        ColoredDrawer(LScriptObject& script)
        {
            hasColor = script.load_property(_color, "Color");
        }

        MX::Color imageColor()
        {
            MX::Color color = _color;
            color.SetA(MX::Widgets::Widget::current().geometry.color.a() * color.a());
            return color;
        }

        bool hasColor = false;
        MX::ScriptableColor _color;
    };

	class ImageDrawer : public ColoredDrawer
	{
	public:
		ImageDrawer() {}
		ImageDrawer(LScriptObject& script) : ColoredDrawer(script)
		{
			script.load_property_child(_image, "Image");
			script.load_property(_position, "Pos");
		}

		void DrawBackground()
		{
			if (MX::Widgets::Widget::current().geometry.color.a() == 0x0)
				return;

			if (!hasImage())
				return;

			DrawImage();
		}

		virtual void DrawImage()
		{
			if (_position == -1)
			{
				float x = Destination::current().x(), y = Destination::current().y();
				image().DrawCentered(0.0f, 0.0f, x, y, 1.0f, 1.0f, MX::Widgets::Widget::current().geometry.angle, imageColor());
				return;
			}


			auto r = MX::Rectangle::fromWH(.0f, .0f, image().Width(), image().Height());
			r.NumLayoutIn(Destination::current().rectangle, _position);
			image().DrawCentered(0.0f, 0.0f, r.x1, r.y1, 1.0f, 1.0f, MX::Widgets::Widget::current().geometry.angle, imageColor());
		}
	protected:


		bool hasImage()
		{
			return _image || Context<MX::Graphic::Image>::isCurrent();
		}

		MX::Graphic::Image& image()
		{
			if (_image)
				return *_image;
			return Context<MX::Graphic::Image>::current();
		}

		MX::Graphic::ImagePtr _image;
		int _position = -1;
	};

	class ColorDrawer : public MX::Widgets::ImageDrawer
	{
	public:
		ColorDrawer(LScriptObject& script);

		void DrawImage();
	};

	class DrawersInit
	{
	public:
		static void Init();
	};


}
}

#endif

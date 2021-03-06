#ifndef MXDRAWERS
#define MXDRAWERS
#include "Drawer.h"
#include "DrawerUtils.h"
#include "graphic/ScriptableColor.h"
#include "graphic/images/Image.h"
#include "graphic/images/Slice9Image.h"
#include "script/ScriptObject.h"
#include "utils/Utils.h"
#include "widgets/Widget.h"

namespace MX
{
namespace Widgets
{

    class ColoredDrawer : public MX::Widgets::Drawer
    {
    protected:
        ColoredDrawer() { }
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
        ImageDrawer() { }
        ImageDrawer(LScriptObject& script)
            : ColoredDrawer(script)
        {
            script.load_property_child(_image, "Image");
            script.load_property(_position, "Pos");
            script.load_property(_angle, "Angle");
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
                image().DrawCentered({}, { x, y }, { 1.0f, 1.0f }, angle(), imageColor());
                return;
            }

            auto r = MX::Rectangle::fromWH(.0f, .0f, image().Width(), image().Height());
            r.NumLayoutIn(Destination::current().rectangle, _position);
            image().DrawCentered({}, { r.x1, r.y1 }, { 1.0f, 1.0f }, angle(), imageColor());
        }

    protected:
        float angle()
        {
            return MX::Widgets::Widget::current().geometry.angle + (_angle ? *_angle : 0.0f);
        }

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

        Scriptable::Value::pointer _angle;
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

#include "Drawers.h"
#include "widgets/Widget.h"
#include "script/ScriptClassParser.h"
#include "script/class/ScriptImageClass.h"
#include "widgets/Label.h"
#include "Game/Resources/Resources.h"
#include "widgets/Label.h"
#include "graphic/images/Slice9Image.h"
#include "utils/ContextStack.h"

using namespace MX;

#ifdef WIPFONT
class TextImageDrawer : public MX::Widgets::Drawer
{
public:
	TextImageDrawer(LScriptObject& script)
	{
		script.load_property(_index, "Index");
		script.load_property(_color, "Color");
		script.load_property(_font, "Font");
		script.load_property(_width, "Width");
		script.load_property(_pos, "Pos");
        script.load_property(_offset, "Offset");

        script.load_property(_offset.x, "Offset.X");
        script.load_property(_offset.y, "Offset.Y");

		//TODO horrible hack to make TextDrawer Prototyping work
		if (!_font)
		{
			std::string name;
			std::string face;
			float size;

			if (script.load_property(name, "Font.Name") &&
				script.load_property(size, "Font.Size"))
			{
				script.load_property(face, "Font.Face");
				_font = MX::Graphic::Font::Create(name, size, face);
			}
		}

		bool center = true;
		if (script.load_property(center, "Center"))
		{
			_pos = center ? 5 : 7;
		}
	}

	void DrawBackground()
	{
		auto &widget = MX::Widgets::Widget::current();
		if (widget.geometry.color.a() != 0x0 && MX::Widgets::TextData::isCurrent())
		{
			auto &dataStack = ContextStack<MX::Widgets::TextData>::stack();
			if (dataStack.size() <= _index)
				return;

			auto &textData = *(dataStack[_index]);

			if (_font)
				textData.SetFont(_font);

			float width = -1.0f;

			if (_width)
				width = *_width;


			if (width != -1.0)
				textData.SetWidth(width);
			else
				textData.SetWidth(Destination::current().rectangle.width());

			auto &image = textData.textImage();
			if (!image)
				return;
			float x = Destination::current().x(), y = Destination::current().y();

			MX::Rectangle source;
			source.SetWidth(image->Width());
			source.SetHeight(image->Height());

			source.NumLayoutIn(Destination::current().rectangle, _pos);

			MX::Color color = _color;
			color.SetA(widget.geometry.color.a() * color.a());

            source.Shift(_offset);

			//clip to int
			float t;
			source.Shift(-std::modf(source.x1, &t), -std::modf(source.y1, &t));
			
			if (textData.HTML())
			{
				auto g = Graphic::Blender::defaultPremultiplied().Use();
				image->Draw(source, color);
			}
			else
				image->Draw(source, color);
		}
	}

protected:

	unsigned _index = 0;
	int _pos = 0;
	MX::ScriptableColor _color;
	Graphic::Font::pointer _font;
	Scriptable::Value::pointer _width;
    glm::vec2 _offset;
};
#endif






class StretchedImageDrawer : public MX::Widgets::ImageDrawer
{
public:
	StretchedImageDrawer(LScriptObject& script) : ImageDrawer(script)
	{
		
	}

	void DrawImage()
	{
		float x = Destination::current().x(), y = Destination::current().y();
		float w1 = Destination::current().width(), h1 = Destination::current().height();

		auto &img = image();
		auto w2 = img.Width(), h2 = img.Height();
		img.DrawCentered( {}, { x, y }, { w1 / w2, h1 / h2 }, MX::Widgets::Widget::current().geometry.angle, imageColor() );
	}
};


class TiledImageDrawer : public MX::Widgets::ImageDrawer
{
public:
	TiledImageDrawer(LScriptObject& script) : ImageDrawer(script)
	{

	}

	void DrawImage()
	{
		float x = Destination::current().x(), y = Destination::current().y();
		auto rect = Destination::current().rectangle;
		rect.Shift(-rect.x1, -rect.y1);
		image().Draw(Destination::current().rectangle, rect, MX::Widgets::Widget::current().geometry.color);
	}
};



class TransformedImageDrawer : public MX::Widgets::ImageDrawer
{
public:

	enum AxisMode
	{
		None = 0,
		Stretch,
		Tile,
		Center,
	};


	static AxisMode stringToAxisMode(const std::string & str)
	{
		static std::map<std::string, AxisMode> mmap = { { "None", None }, { "Stretch", Stretch }, { "Tile", Tile },{ "center", Center } };
		auto it = mmap.find(str);
		if (it != mmap.end())
			return it->second;
		return None;
	}

	TransformedImageDrawer(LScriptObject& script) : ImageDrawer(script)
	{

		std::string aX, aY;
		bool loadedX = script.load_property(aX, "AxisX");
		bool loadedY = script.load_property(aY, "AxisY");
		_axisX = stringToAxisMode(aX);
		_axisY = stringToAxisMode(aY);

		script.load_property(_flipX, "FlipX");
		script.load_property(_flipY, "FlipY");
		
		if (script.load_property(_position, "Pos"))
		{
			if (!loadedX)
				_axisX = Center;
			if (!loadedY)
				_axisY = Center;
		}


		script.load_property(_scale, "Scale");
	}

	void DrawImage()
	{
		float x = Destination::current().x(), y = Destination::current().y();
		auto &img = image();

		auto destination = Destination::current().rectangle;
		MX::Rectangle source;


		if (_axisX == None)
		{
			source.SetWidth(img.Width());
			destination.SetWidth(img.Width() * _scale.x);
		}
		else if (_axisX == Stretch)
		{
			source.SetWidth(img.Width());
		}
		else if (_axisX == Tile)
		{
			source.SetWidth(destination.width());
		}
		else if (_axisX == Center)
		{
			auto center = Destination::current().rectangle.center();
			source.SetWidth(img.Width());
			auto r2 = source;
			r2.SetWidth(r2.width() * _scale.x);
			r2.NumLayoutIn(destination, _position);
			destination.x1 = r2.x1;
			destination.x2 = r2.x2;
		}

		if (_axisY == None)
		{
			source.SetHeight(img.Height());
			destination.SetHeight(img.Height() * _scale.y);
		}
		else if (_axisY == Stretch)
		{
			source.SetHeight(img.Height());
		}
		else if (_axisY == Tile)
		{
			source.SetHeight(destination.height());
		}
		else if (_axisY == Center)
		{
			auto center = Destination::current().rectangle.center();
			source.SetHeight(img.Height());
			auto r2 = source;
			r2.SetHeight(r2.height() * _scale.x);
			r2.NumLayoutIn(destination, _position);
			destination.y1 = r2.y1;
			destination.y2 = r2.y2;
		}

		if (_flipX)
			std::swap(source.x1, source.x2), source.x2 -= 1.0f, source.x1 += 1.0f;
		if (_flipY)
			std::swap(source.y1, source.y2), source.y2 -= 1.0f, source.y1 += 1.0f;
		img.Draw(destination, source, imageColor());

	}

protected:
	AxisMode _axisX = None;
	AxisMode _axisY = None;
	bool _flipX = false;
	bool _flipY = false;

	glm::vec2 _scale = { 1.0f, 1.0f };

	int _position = 5;
};


MX::Widgets::ColorDrawer::ColorDrawer(LScriptObject& script) : ImageDrawer(script)
{
	if (!_image)
		_image = MX::Resources::get().loadImage("Misc/White128.png");
}

void MX::Widgets::ColorDrawer::DrawImage()
{
	if (!Context<Drawer::ContextData>::isCurrent())
		return;

	auto &img = image();
	float x = Destination::current().x(), y = Destination::current().y();
	float w1 = Destination::current().width(), h1 = Destination::current().height();
	auto w2 = img.Width(), h2 = img.Height();


	img.DrawCentered( {}, { x, y }, { w1 / w2, h1 / h2 }, MX::Widgets::Widget::current().geometry.angle, imageColor() );
}



class QuadImageDrawer : public MX::Widgets::ColoredDrawer
{
public:
	QuadImageDrawer(LScriptObject& script) : MX::Widgets::ColoredDrawer(script)
	{
		script.load_property_child(_image, "Image");
        script.load_property(_flipX, "flipX");
        script.load_property(_flipY, "flipY");
	}
	void DrawBackground()
	{
        if (!Context<Drawer::ContextData>::isCurrent())
            return;

		if (MX::Widgets::Widget::current().geometry.color.a() != 0x0 && _image)
		{
            bool oldX = Graphic::Image::Settings::flipX;
            bool oldY = Graphic::Image::Settings::flipY;

            Graphic::Image::Settings::flipX = _flipX;
            Graphic::Image::Settings::flipY = _flipY;

            _image->Draw(Destination::current().rectangle, imageColor());

            Graphic::Image::Settings::flipX = oldX;
            Graphic::Image::Settings::flipY = oldY;
			
		}
	}


protected:
    bool _flipX = false;
    bool _flipY = false;

	std::shared_ptr<Graphic::QuadImage> _image;
};


class DebugColorDrawer : public MX::Widgets::ColorDrawer
{
public:
    using ColorDrawer::ColorDrawer;

    void DrawImage() override
    {
        if (!Context<Drawer::ContextData>::isCurrent())
            return;
        if (!Context<Drawer::ContextData>::current().drawDebugDrawers)
            return;
        ColorDrawer::DrawImage();
    }
};



void MX::Widgets::DrawersInit::Init()
{
#ifdef WIPFONT
	ScriptClassParser::AddCreator(L"Drawer.TextImage", new OutsideScriptClassCreatorContructor<TextImageDrawer>());
#endif
	ScriptClassParser::AddCreator(L"Drawer.Image", new OutsideScriptClassCreatorContructor<ImageDrawer>());
	ScriptClassParser::AddCreator(L"Drawer.TransformedImage", new OutsideScriptClassCreatorContructor<TransformedImageDrawer>());
	ScriptClassParser::AddCreator(L"Drawer.StretchedImage", new OutsideScriptClassCreatorContructor<StretchedImageDrawer>());
	ScriptClassParser::AddCreator(L"Drawer.TiledImage", new OutsideScriptClassCreatorContructor<TiledImageDrawer>());
	ScriptClassParser::AddCreator(L"Drawer.Color", new OutsideScriptClassCreatorContructor<ColorDrawer>());
	ScriptClassParser::AddCreator(L"Drawer.QuadImage", new OutsideScriptClassCreatorContructor<QuadImageDrawer>());
    ScriptClassParser::AddCreator(L"Drawer.Debug.Color", new OutsideScriptClassCreatorContructor<DebugColorDrawer>());
}
#include "ScriptImageClass.h"
#include "script/Script.h"
#include "Game/Resources/Resources.h"
#include "graphic/Blender.h"
#include "script/PropertyLoaders.h"
#include <iostream>
using namespace MX;





ScriptImageClass::ScriptImageClass()
{
}

std::shared_ptr<Graphic::Image> ScriptImageClass::image() const
{
	return _image;
}

bool ScriptImageClass::onParse()
{
	//_sound
	std::string path;
	load_property_child(path, "Path");
	load_property(_pivot, "Pivot");
	load_property(MX::Graphic::Image::Settings::mipmap, "Mipmap");

	
	MX::Graphic::Blender blender(0, 0, 0);
	if (load_property(blender, "Blend"))
	{
		assert( false );
		//_image = Resources::get().loadCenteredBlenderImage(_pivot.x, _pivot.y, blender, path.c_str());
		return true;
	}


	bool old_pre = MX::Graphic::Image::Settings::premultiplied_alpha;
	load_property(MX::Graphic::Image::Settings::premultiplied_alpha, "PremultipliedAlpha", true);

	_image = Resources::get().loadCenteredImage(_pivot.x, _pivot.y, path.c_str());
	MX::Graphic::Image::Settings::premultiplied_alpha = old_pre;

	bool center = false;
	load_property(center, "Center");

	if (center)
		_image->SetCenter({ _image->Width() / 2.0f, _image->Height() / 2.0f });


	bool wrap = false;
	load_property(wrap, "Wrap");
	if ( wrap )
	{
		assert( false );
#ifdef WIP
		_image->native_bitmap_handle()->setWrap(GL_REPEAT, GL_REPEAT);
#endif
	}
	MX::Graphic::Image::Settings::mipmap = false;

    if ( !_image )
    {
        //WIPLOG
        std::cout << "cannot load image " << path << "\n";
        //assert(_image);
    }
	
	return true;
}



ScriptSlice9ImageClass::ScriptSlice9ImageClass()
{
}

std::shared_ptr<Graphic::QuadImage> ScriptSlice9ImageClass::image() const
{
	return _image;
}


bool ScriptSlice9ImageClass::onParse()
{
	//_sound


	std::string filenameTemplate;
	if (load_property(filenameTemplate, "ImagesPath"))
	{
		std::vector<Graphic::ImagePtr> images;
		

		char tmp[256];
		for (unsigned i = 0; i < 9; i++)
		{
			sprintf(tmp, filenameTemplate.c_str(), i);
			auto image = Resources::get().loadImage(tmp);
			if (!image)
				return false;
			images.push_back(image);
		}
		_image = std::make_shared<Graphic::Slice9Image>(images);
		return true;
	}


	std::string path;
	load_property(path, "Path");
	MX::Rectangle rect;
	load_property(rect, "Margins");

	_image = std::make_shared<Graphic::Slice9Image>(Margins::fromRectangle(rect), Resources::get().loadImage(path.c_str()));
	return true;
}



Graphic::TextureImage::pointer ScriptImageGridClass::image(int x, int y) const
{
	auto surf = MX::Graphic::TextureImage::Create(_image, Rectangle::fromWH(x*_gridSize.x, y*_gridSize.y, _gridSize.x, _gridSize.y));
	surf->SetCenter(_pivot);
	return surf;
}

bool ScriptImageGridClass::onParse()
{
	//_sound
	if (!ScriptImageClass::onParse())
		return false;

	load_property(_gridSize, "GridSize");
	_columns = ScriptImageClass::image()->Width() / _gridSize.x;
	_rows = ScriptImageClass::image()->Height() / _gridSize.y;


	return true;
}

bool ScriptImageFromGridClass::onParse()
{

	std::pair<unsigned, unsigned> pos;
	load_property(pos, "Pos");

	auto grid = property_pointer<ScriptImageGridClass>("Grid");
	if (!grid)
		return false;

	_image = grid->image(pos.first, pos.second);
	return true;
}

ScriptRandomImage::ScriptRandomImage()
{
}

std::shared_ptr<Graphic::Image> ScriptRandomImage::image() const
{
	static std::shared_ptr<Graphic::TextureImage> dummy;
	 if (_images.empty())
		 return dummy;
	 return _images[rand() % _images.size()];
}


bool ScriptRandomImage::onParse()
{
	load_property(_images, "Images");

	unsigned framesOffset = 0, framesCount = 0;
	load_property(framesOffset, "Images_offset");
	load_property(framesCount, "Images_count");

	std::string filenameTemplate;
	load_property(filenameTemplate, "Filename_template");

	char tmp[256];
	for (unsigned i = framesOffset; i < framesCount; i ++)
	{
		sprintf(tmp, filenameTemplate.c_str(),i); 
		auto image = Resources::get().loadCenteredImage(_pivot.x, _pivot.y, tmp);
		if (image)
			_images.push_back(image);
	}

	return true; 
}


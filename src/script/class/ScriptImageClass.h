#pragma once
#include <string>
#include "utils/Utils.h"
#include "script/ScriptClass.h"
#include "graphic/images/TextureImage.h"
#include "graphic/images/Slice9Image.h"

namespace MX{



class ScriptImageClass : public ScriptClass
{
public:
	ScriptImageClass();

	virtual std::shared_ptr<Graphic::Image> image() const;
protected:
	bool onParse() override;

	Graphic::TextureImage::pointer _image;
	glm::vec2 _pivot;
};

class ScriptSlice9ImageClass : public ScriptClass
{
public:
	ScriptSlice9ImageClass();

	virtual std::shared_ptr<Graphic::QuadImage> image() const;
protected:
	bool onParse() override;

	std::shared_ptr<Graphic::QuadImage> _image;
};

class ScriptImageFromGridClass : public ScriptImageClass
{
protected:
	bool onParse() override;
};

class ScriptImageGridClass : public ScriptImageClass
{
public:
	virtual Graphic::TextureImage::pointer image(int x, int y) const;
protected:
	bool onParse() override;

	unsigned _rows = 1, _columns = 1;
	glm::vec2 _gridSize;
};



class ScriptRandomImage : public ScriptImageClass
{
public:
	ScriptRandomImage();

	std::shared_ptr<Graphic::Image> image() const override;
protected:
	bool onParse() override;

	std::vector<std::shared_ptr<Graphic::Image>> _images;

	
};





}

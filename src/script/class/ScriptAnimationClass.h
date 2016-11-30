#pragma once
#include <string>
#include "ScriptImageClass.h"
#include "utils/Utils.h"
#include "script/ScriptClass.h"
#include "graphic/animation/SingleAnimation.h"

namespace MX{



class ScriptAnimationClass : public ScriptImageClass
{
public:
	ScriptAnimationClass();

	virtual const std::shared_ptr<Graphic::SingleAnimation>& animation() const;
	std::shared_ptr<Graphic::Image> image() const;
protected:

	std::shared_ptr<Graphic::SingleAnimation> _animation;
};

class ScriptAnimationFromFileClass : public ScriptAnimationClass
{
public:
	ScriptAnimationFromFileClass();
protected:
	bool onParse();	
};

class ScriptAnimationFromFilesClass : public ScriptAnimationClass
{
public:
	ScriptAnimationFromFilesClass();
protected:
	bool onParse();

	float _frameDuration = 0.0f;
	unsigned _framesCount = 0;
	std::string _filenameTemplate;
	glm::vec2 _pivot;

	
};

class ScriptRandomAnimation : public ScriptAnimationClass
{
public:
	ScriptRandomAnimation();

	 const std::shared_ptr<Graphic::SingleAnimation>& animation() const;
protected:
	bool onParse();

	std::vector<std::shared_ptr<Graphic::SingleAnimation>> _animations;

	
};

}

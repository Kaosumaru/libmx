#pragma once
#include "Scene/Sprites/SpriteScene.h"
#include "Utils/Time.h"
#include <list>
#include "graphic/images/TextureImage.h"

namespace MX
{

class SceneTransition : public virtual SpriteActor
{
public:
	virtual void SetScenes(const std::shared_ptr<SpriteScene> &old_scene, const std::shared_ptr<SpriteScene> &new_scene){}
};


class BaseBitmapTransition : public virtual SceneTransition
{
public:
	BaseBitmapTransition(float time = 0.5f);
	void SetScenes(const std::shared_ptr<SpriteScene> &old_scene, const std::shared_ptr<SpriteScene> &new_scene);
	void Run();
	void Draw(float x, float y);
protected:
	Graphic::TextureImage::pointer _scene1;
	Graphic::TextureImage::pointer _scene2;
	Time::ManualStopWatch    _stopWatch;
	double                   _time;
};

class AlphaBitmapTransition : public virtual BaseBitmapTransition
{
public:
	AlphaBitmapTransition(float time = 0.5f);
	void Draw(float x, float y);
};

class MoveBitmapTransition : public virtual BaseBitmapTransition
{
public:
	MoveBitmapTransition(bool left = true, float time = 0.5f);
	void Draw(float x, float y);
protected:
	bool _left;
};

}

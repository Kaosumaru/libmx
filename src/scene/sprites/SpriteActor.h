#pragma once
#include <memory>
#include "utils/Vector2.h"
#include "scene/Actor.h"
#include "graphic/images/Image.h"
#include "utils/Time.h"
#include "graphic/animation/Animation.h"

namespace MX
{

class SpriteScene;
class SpriteActor;
typedef std::shared_ptr<SpriteActor> SpriteActorPtr;

class SpriteActor : public virtual Actor
{
public:
	SpriteActor();
	~SpriteActor();

	virtual void Draw(float x, float y) {}
	virtual void DrawCustom(float x, float y) {}
	SpriteScene &sprite_scene();
	glm::vec2 absolute_position();

	virtual SpriteActorPtr clone() { return nullptr; }
	std::shared_ptr<Actor> cloneActor() { return clone(); }

	//Implementations can mark this element as inactive, and Unlink it later
	//(ie particle effects can unlink self after all particles are dead)
	virtual void StopThenUnlink() { Unlink(); }

	struct Geometry
	{
		glm::vec2 position;
		glm::vec2 scale;
		float z = 0.0f;
		float angle = 0.0f;
		ColorExtended color;
	} geometry;

#ifdef _DEBUG
	static int _liveCounter;
#endif
};



template <typename T>
class ImageSpriteAdapter : public T
{
public:
	ImageSpriteAdapter(const std::shared_ptr<Graphic::Image> &image = nullptr) : _image(image)
	{
	}

	template<typename X>
	ImageSpriteAdapter(X& t, const std::shared_ptr<Graphic::Image> &image) : _image(image), T(t)
	{
	}


	void Draw(float x, float y)
	{
		if (_image)
			_image->DrawCentered({}, { x, y }, T::geometry.scale, T::geometry.angle, T::geometry.color);
	}

	void SetImage(const std::shared_ptr<Graphic::Image> &image)
	{
		_image = image;
	}
protected:
	std::shared_ptr<Graphic::Image> _image;
};

template <typename T>
class AnimatedSpriteAdapter : public T
{
public:
	AnimatedSpriteAdapter(const Graphic::Animation::pointer &animation = nullptr) :_animation(animation)
	{
	}

	template<typename X>
	AnimatedSpriteAdapter(X& t, const Graphic::Animation::pointer &animation = nullptr) :_animation(animation), T(t)
	{
	}

	void Run()
	{
		_animation->AdvanceTime(Time::Timer::current().elapsed_seconds());
		T::Run();
	}
	void Draw(float x, float y)
	{
		_animation->DrawCentered({}, { x, y }, T::geometry.scale, T::geometry.angle, T::geometry.color);
	}

	void SetAnimation(const std::shared_ptr<Graphic::Animation> &animation)
	{
		_animation = animation;
	}
protected:
	Graphic::Animation::pointer _animation;
};

template <typename T>
class SingleRunAnimatedSpriteAdapter : public T
{
public:
	SingleRunAnimatedSpriteAdapter(const std::shared_ptr<Graphic::Animation> &animation = nullptr, bool freeze_last_frame = false) :_animation(animation)
	{
		_freeze_last_frame = freeze_last_frame;
	}

	template<typename X>
	SingleRunAnimatedSpriteAdapter(X& t, const std::shared_ptr<Graphic::Animation> &animation = nullptr, bool freeze_last_frame = false) :_animation(animation), T(t)
	{
		_freeze_last_frame = freeze_last_frame;
	}



	void Run()
	{
		_animation->AdvanceTime(Time::Timer::current().elapsed_seconds());
		if (!_animation->running())
		{
            onLastFrame();
            if (!_freeze_last_frame)
                T::Unlink();
		}
		T::Run();
	}
	void Draw(float x, float y)
	{
		_animation->DrawCentered({}, { x, y }, T::geometry.scale, T::geometry.angle, T::geometry.color);
	}

	void SetImage(const std::shared_ptr<Graphic::Animation> &animation)
	{
		_animation = animation;
	}
protected:
    virtual void onLastFrame()
    {
        
    }
    
	Graphic::Animation::pointer _animation;
	bool _freeze_last_frame;
};

using ImageSpriteActor = ImageSpriteAdapter<SpriteActor>;
using SingleRunAnimatedSpriteActor = SingleRunAnimatedSpriteAdapter<SpriteActor> ;
using AnimatedSpriteActor = AnimatedSpriteAdapter<SpriteActor>;

}
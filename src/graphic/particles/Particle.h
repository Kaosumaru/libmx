#pragma once
#include "scene/sprites/SpriteScene.h"
#include "graphic/images/Image.h"
#include "script/ScriptObject.h"
#include "utils/Time.h"
#include "utils/Random.h"
#include "utils/Utils.h"
#include "script/class/ScriptImageClass.h"
#include "script/class/ScriptAnimationClass.h"

namespace MX{
namespace Graphic{


class Particle 
{
public:
	Particle()
	{
		_angle1 = _angle2 = 0.0f;
	}

	virtual ~Particle() {}

	virtual bool Run()
	{
		if (_stopWatch.Tick())
		{
			return false;
		}
		auto elapsed_seconds = Time::Timer::current().elapsed_seconds();
		auto percent = _stopWatch.percent();

		geometry.position += _directionForce * elapsed_seconds;
		geometry.scale = lerp(_scale1, _scale2, percent);

		double _particleColorInactivePercent = _timeToColorChange / _stopWatch.total_time();
		auto lerp_percent = percent - (percent > _particleColorInactivePercent ? _particleColorInactivePercent : 0.0f);
		geometry.color = Color::lerp(_color1, _color2, lerp_percent);


		geometry.angle += _angular_vel * elapsed_seconds;
		_directionForce += _acceleration * elapsed_seconds;
		return true;
	}

	virtual void Draw(float x, float y) const
	{

	}

	virtual void DrawCustom(float x, float y) const
	{

	}

	virtual Graphic::Image* baseImage() const { return nullptr; }
	virtual Graphic::Image* normalsImage() const { return nullptr; }

	glm::vec2              _directionForce;
	glm::vec2              _acceleration;
	float				 _angular_vel;
	double				 _timeToColorChange = 0.0;

	glm::vec2              _scale1, _scale2;
	float                _angle1, _angle2;
	Color                _color1, _color2;
	Time::ManualStopWatchAbsolute      _stopWatch;


	SpriteActor::Geometry geometry;
};


class ParticleImage : public Particle
{
public:
	ParticleImage(const std::shared_ptr<Graphic::Image> &image = nullptr) : _image(image.get())
	{
	}

	void Draw(float x, float y)  const override
	{
		if (_image)
			_image->DrawCentered( {}, { x, y }, geometry.scale, geometry.angle, geometry.color );
	}

	void SetImage(const std::shared_ptr<Graphic::Image> &image)
	{
		_image = image.get();
	}

	Graphic::Image* baseImage() const override 
	{ 
		return _image;
	}
protected:
	Graphic::Image* _image;
};




class ParticleCreator : public shared_ptr_init<ParticleCreator>, public ScriptObjectString
{
protected:

public:
	ParticleCreator(const std::string& objectName) : ScriptObjectString(objectName) 
	{
		load_property(_scale1, "Start.Scale");
		load_property(_scale2, "End.Scale");

		load_property(_color1, "Start.Color");
		load_property(_color2, "End.Color");;

		load_property(_angle1, "Start.Angle");
		load_property(_angle2, "End.Angle");

		load_property(_angular_vel, "AngularVelocity");
	}

	virtual std::unique_ptr<Particle> CreateParticle() = 0;
	virtual ParticleCreator::pointer clone()
	{
		return shared_from_this();
	}

protected:
	virtual void InitParticle(Particle &p)
	{
		p._scale1 = lerp(_scale1.first, _scale1.second, Random::randomRange());
		p._scale2 = lerp(_scale2.first, _scale2.second, Random::randomRange());
		p._color1 = Color::lerp(_color1.first, _color1.second, Random::randomRange());
		p._color2 = Color::lerp(_color2.first, _color2.second, Random::randomRange());

		p._angle1 = Random::randomRange(_angle1);
		p._angle2 = Random::randomRange(_angle2);

		p._angular_vel = Random::randomRange(_angular_vel);
	}

	std::pair<Color, Color> _color1, _color2;
	std::pair<glm::vec2, glm::vec2> _scale1, _scale2;
	std::pair<float, float> _angle1, _angle2;
	std::pair<float, float> _angular_vel;
};





class ParticleCreatorInit
{
public:
	static void Init();
};


}
}
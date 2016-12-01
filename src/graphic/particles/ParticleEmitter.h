#pragma once
#include "Scene/Sprites/SpriteScene.h"
#include "Graphic/Images/Image.h"
#include "Script/ScriptObject.h"
#include "Utils/Time.h"
#include "Utils/Utils.h"
#include "Particle.h"
#include "Particles.h"
#include "utils/shapes/Circle.h"

namespace MX{
namespace Graphic{

class ParticleSystem;

class ParticleEmitter : public ScriptObjectString, public shared_ptr_init<ParticleEmitter>
{
public:
	ParticleEmitter(){}
	ParticleEmitter(const std::string& objectName) : ScriptObjectString(objectName) 
	{

	}

	virtual void EmitParticle(Particle &p, ParticleSystem &s)
	{

	}

	virtual bool isStateless() { return true; }
	virtual ParticleEmitter::pointer clone() { return shared_from_this(); }
protected:
	std::pair<float, float> _life, _speed, _acceleration;
};

class ParticleEmitterInit
{
public:
	static void Init();
};



class ParticleEmitterBase : public ParticleEmitter
{
public:
	ParticleEmitterBase(){}
	ParticleEmitterBase(const std::string& objectName) : ParticleEmitter(objectName)
	{
		load_property(_life, "Life");
		load_property(_speed, "Speed");
		load_property(_acceleration, "Acceleration");
	}

	void EmitParticle(Particle &p, ParticleSystem &s) override
	{
		p._stopWatch.Start(Random::randomRange(_life));
	}
protected:
	std::pair<float, float> _life, _speed, _acceleration;
};


class PointParticleEmitter : public ParticleEmitterBase
{
public:
	PointParticleEmitter(const std::string& objectName) : ParticleEmitterBase(objectName)
	{
		load_property(_direction, "Direction");
		_direction.first *= (float)MX_2PI;
		_direction.second *= (float)MX_2PI;
	}


	void EmitParticle(Particle &p, ParticleSystem &s) override
	{
		ParticleEmitter::EmitParticle(p, s);
		p._stopWatch.Start(Random::randomRange(_life));
		p._directionForce = CreateVectorFromAngle(Random::randomRange(_direction)) * Random::randomRange(_speed);
		p._acceleration = CreateVectorFromAngle(Random::randomRange(_direction)) * Random::randomRange(_acceleration);
	}

protected:
	std::pair<float, float> _direction;
};


class PointRotationParticleEmitter : public ParticleEmitterBase
{
public:
	PointRotationParticleEmitter(const std::string& objectName) : ParticleEmitterBase(objectName)
	{
		load_property(_direction_range, "Direction");
		_direction_range.first *= (float)MX_2PI;
		_direction_range.second *= (float)MX_2PI;
	}


	void EmitParticle(Particle &p, ParticleSystem &s) override
	{
		ParticleEmitter::EmitParticle(p, s);
		float direction = Random::randomRange(_direction_range);
		p._stopWatch.Start(Random::randomRange(_life));
		p._directionForce = CreateVectorFromAngle(direction) * Random::randomRange(_speed);
		p._acceleration = CreateVectorFromAngle(direction) * Random::randomRange(_acceleration);
		p.geometry.angle += direction;
	}

protected:
	std::pair<float, float> _direction_range;

};



class PointGaussParticleEmitter : public ParticleEmitterBase
{
public:
	PointGaussParticleEmitter(const std::string& objectName) : ParticleEmitterBase(objectName)
	{
		load_property(_direction, "Direction");
		_direction.first *= (float)MX_2PI;
		_direction.second *= (float)MX_2PI;
	}


	void EmitParticle(Particle &p, ParticleSystem &s) override
	{
		ParticleEmitter::EmitParticle(p, s);
		p._stopWatch.Start(Random::randomRange(_life));
		p._directionForce = CreateVectorFromAngle(Random::randomGaussRange(_direction)) * Random::randomRange(_speed);
		p._acceleration = CreateVectorFromAngle(Random::randomGaussRange(_direction)) * Random::randomRange(_acceleration);
	}

protected:
	std::pair<float, float> _direction;
};




class CircleParticleEmitter : public PointParticleEmitter
{
public:
	CircleParticleEmitter(const std::string& objectName) : PointParticleEmitter(objectName)
	{
		load_property(_radius, "Radius");
		load_property(_creationAngle, "CreationAngle");

	}

	void EmitParticle(Particle &p, ParticleSystem &s) override
	{
		PointParticleEmitter::EmitParticle(p, s);
		Circle circ(_radius);
		p.geometry.position += circ.RandomPointInside();
	}
protected:
	float _radius = 0.0f;
	std::pair<float, float> _creationAngle;
};

class CircumferenceParticleEmitter : public CircleParticleEmitter
{
public:
	CircumferenceParticleEmitter(const std::string& objectName) : CircleParticleEmitter(objectName)
	{

	}

	void EmitParticle(Particle &p, ParticleSystem &s) override
	{
		PointParticleEmitter::EmitParticle(p, s);
		Circle circ(_radius);
		p.geometry.position += circ.RandomPointCircumference(_creationAngle);
	}
};



class CircleRotationParticleEmitter : public ParticleEmitterBase
{
public:
	CircleRotationParticleEmitter(const std::string& objectName) : ParticleEmitterBase(objectName)
	{
		load_property(_direction_range, "Direction");
		_direction_range.first *= (float)MX_2PI;
		_direction_range.second *= (float)MX_2PI;
		load_property(_radius, "Radius");
	}


	void EmitParticle(Particle &p, ParticleSystem &s) override
	{
		ParticleEmitter::EmitParticle(p, s);
		Circle circ(_radius);
		p.geometry.position += circ.RandomPointInside();
		_direction = Random::randomRange(_direction_range);
		p._stopWatch.Start(Random::randomRange(_life));
		p._directionForce = CreateVectorFromAngle(_direction) * Random::randomRange(_speed);
		p._acceleration = CreateVectorFromAngle(_direction) * Random::randomRange(_acceleration);
		p.geometry.angle += _direction;

	}

protected:
	std::pair<float, float> _direction_range;
	float _direction = 1.0f;
	float _radius = 0.0f;

};


}
}

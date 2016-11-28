#include "MXParticle.h"
#include "Script/MXScriptClassParser.h"
#include "Utils/Pools/PooledObject.h"


using namespace MX;
using namespace MX::Graphic;



class ParticleImageCreator : public ParticleCreator
{
public:

	ParticleImageCreator(const std::string& objectName) : ParticleCreator(objectName)
	{
		load_property(_image, "Image");
	}


	std::unique_ptr<Particle> CreateParticle() override
	{
		auto t = sizeof(ParticleImage);

		auto particle = new PooledObject<ParticleImage>(_image);
		InitParticle(*particle);
		return std::unique_ptr<Particle>(particle);
	};

protected:
	std::shared_ptr<Image> _image;
};


class ParticleRandomImageCreator : public ParticleCreator
{
public:

	ParticleRandomImageCreator(const std::string& objectName) : ParticleCreator(objectName)
	{
		load_property(_animation, "Animation");
	}


	std::unique_ptr<Particle> CreateParticle() override
	{
		auto particle = new PooledObject<ParticleImage>(_animation->random_frame().image);
		InitParticle(*particle);
		return std::unique_ptr<Particle>(particle);
	};

protected:
	Graphic::SingleAnimation::pointer _animation;
};



class ParticleImageFlying : public ParticleImage
{
public:
	ParticleImageFlying(const std::shared_ptr<Graphic::Image> &image) : ParticleImage(image)
	{

	}

	virtual bool Run()
	{
		if (!Particle::Run())
			return false;
		auto elapsed_seconds = Time::Timer::current().elapsed_seconds();
		auto percent = _stopWatch.percent();

		float scale_percent = sin(percent * MX_PI) * _scale_at_top_height;
		geometry.scale.x *= scale_percent;
		geometry.scale.y *= scale_percent;


		if (!_directionForce.zero() && _deceleration)
			_directionForce.approach_zero(_deceleration * elapsed_seconds);

		return true;
	}

	float _deceleration = 0.0f;
	float _scale_at_top_height = 0.0f;
};


class ParticleImageFlyingCreator : public ParticleCreator
{
public:

	ParticleImageFlyingCreator(const std::string& objectName) : ParticleCreator(objectName)
	{
		load_property(_animation, "Animation");
		load_property(_image, "Image");

		load_property(_scaleAtTopHeight, "ScaleAtTopHeight");
		load_property(_deceleration, "Deceleration");
	}


	std::unique_ptr<Particle> CreateParticle() override
	{
		auto particle = new PooledObject<ParticleImageFlying>(_animation ? _animation->random_frame().image : _image);
		InitParticle(*particle);
		particle->_scale_at_top_height = Random::randomRange(_scaleAtTopHeight);
		return std::unique_ptr<Particle>(particle);
	};

protected:
	Graphic::SingleAnimation::pointer _animation;
	std::shared_ptr<Image> _image;

	std::pair<float, float> _scaleAtTopHeight;
	std::pair<float, float> _deceleration;
};



void ParticleCreatorInit::Init()
{
	ScriptClassParser::AddCreator(L"Particles.Creator.Image", new DefaultClassCreatorContructor<ParticleImageCreator>());
	ScriptClassParser::AddCreator(L"Particles.Creator.Image.Flying", new DefaultClassCreatorContructor<ParticleImageFlyingCreator>());
	ScriptClassParser::AddCreator(L"Particles.Creator.RandomImage", new DefaultClassCreatorContructor<ParticleRandomImageCreator>());
}
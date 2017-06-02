#include "Particle.h"
#include "script/ScriptClassParser.h"
#include "utils/pools/PooledObject.h"
#include "utils/Vector2.h"

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

	void approach_zero(glm::vec2& v, float force)
	{
		auto vector = -(normalize(v) * force);

		if (v.x >= 0.0f != (v.x + vector.x) >= 0.0f)
			v.x = 0.0f;
		else
			v.x += vector.x;

		if (v.y >= 0.0f != (v.y + vector.y) >= 0.0f)
			v.y = 0.0f;
		else
			v.y += vector.y;
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

        if ( _directionForce != glm::vec2{0.0f, 0.0f} && _deceleration )
			approach_zero(_directionForce, _deceleration * elapsed_seconds);

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
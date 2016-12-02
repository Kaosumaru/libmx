#include "ParticleEmitter.h"
#include "script/ScriptClassParser.h"


using namespace MX;
using namespace MX::Graphic;


class ParticleEmitterAlignAngle : public ParticleEmitter
{
public:
	ParticleEmitterAlignAngle(){}
	ParticleEmitterAlignAngle(const std::string& objectName) : ParticleEmitter(objectName)
	{

	}

	void EmitParticle(Particle &p, ParticleSystem &s) override
	{
		p.geometry.angle += s.geometry.angle;
	}
};


class ParticleEmitterComposite : public ParticleEmitter
{
public:
	ParticleEmitterComposite(const ParticleEmitterComposite& other)
	{
		for (auto &emitter : other._emitters)
			_emitters.push_back(emitter->clone());
		_stateless = other._stateless;
	}
	ParticleEmitterComposite(const std::string& objectName) : ParticleEmitter(objectName)
	{
		load_property(_emitters, "Emitters");

		for (auto &emitter : _emitters)
			if (!emitter->isStateless())
			{
				_stateless = false;
				break;
			}
				
	}

	void EmitParticle(Particle &p, ParticleSystem &s) override
	{
		for (auto &emitter : _emitters)
			emitter->EmitParticle(p, s);
	}

	ParticleEmitter::pointer clone() override
	{ 
		if (_stateless)
			return shared_from_this(); 
		return std::make_shared<ParticleEmitterComposite>(*this);

	}
protected:
	bool _stateless = true;
	std::list<ParticleEmitter::pointer> _emitters;
};



void ParticleEmitterInit::Init()
{
	ScriptClassParser::AddCreator(L"Particles.Emitter.Point", new DefaultClassCreatorContructor<PointParticleEmitter>());
	ScriptClassParser::AddCreator(L"Particles.Emitter.PointRotation", new DefaultClassCreatorContructor<PointRotationParticleEmitter>());
	ScriptClassParser::AddCreator(L"Particles.Emitter.PointGauss", new DefaultClassCreatorContructor<PointGaussParticleEmitter>());
	ScriptClassParser::AddCreator(L"Particles.Emitter.Circle", new DefaultClassCreatorContructor<CircleParticleEmitter>());
	ScriptClassParser::AddCreator(L"Particles.Emitter.Circumference", new DefaultClassCreatorContructor<CircumferenceParticleEmitter>());
	ScriptClassParser::AddCreator(L"Particles.Emitter.CircleRotation", new DefaultClassCreatorContructor<CircleRotationParticleEmitter>());


	ScriptClassParser::AddCreator(L"Particles.Emitter.AlignAngle", new DefaultClassCreatorContructor<ParticleEmitterAlignAngle>());
	ScriptClassParser::AddCreator(L"Particles.Emitter.Composite", new DefaultClassCreatorContructor<ParticleEmitterComposite>());
}
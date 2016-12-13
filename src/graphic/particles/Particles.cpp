#include "Particles.h"
#include "utils/Random.h"
#include "scene/sprites/ScriptableSpriteActor.h"

#include "Particle.h"
#include "ParticleGenerator.h"
#include "ParticleEmitter.h"
#include "ParticleDecorator.h"
#include "script/ScriptClassParser.h"
#include "graphic/Blender.h"
#include "graphic/renderer/MVP.h"

using namespace MX;
using namespace MX::Graphic;

void ParticleContainer::Run()
{
	auto it = _particles.begin();
	while (it != _particles.end())
	{
		if ((*it)->Run())
			++it;
		else
			it = _particles.erase(it);
	}
}

void ParticleContainer::Run(ParticleSystem *system, const std::shared_ptr<ParticleDecorator>& decorator)
{
	auto it = _particles.begin();
	while (it != _particles.end())
	{
		bool alive = (*it)->Run();
		decorator->DecorateParticle(*(it->get()), *system, alive);

		if (alive)
			++it;
		else
			it = _particles.erase(it);
	}
}
void ParticleContainer::Draw(float x, float y)
{
	for (auto &particle : _particles)
		particle->Draw(particle->geometry.position.x + x, particle->geometry.position.y + y);
}

void ParticleContainer::DrawCustom(float x, float y)
{
	for (auto &particle : _particles)
		particle->DrawCustom(particle->geometry.position.x + x, particle->geometry.position.y + y);
}

void ParticleContainer::RunAndDraw(float x, float y)
{
	auto it = _particles.begin();
	while (it != _particles.end())
	{
		if ((*it)->Run())
		{
			(*it)->Draw((*it)->geometry.position.x + x, (*it)->geometry.position.y + y);
			++it;
		}
		else
			it = _particles.erase(it);
	}
}

ParticleSystem::ParticleSystem(const ParticleSystem& other) : ScriptableSpriteActor(other)
{
	_relative = other._relative;
	_additiveBlending = other._additiveBlending;

	_emitter = other._emitter->clone();
	_generator = other._generator->clone();
	_creator = other._creator->clone();

	if (other._decorator)
		_decorator = other._decorator->isStateless() ? other._decorator : other._decorator->clone();

	assert(_emitter);
	assert(_creator);
	assert(_generator);
}


ParticleSystem::ParticleSystem(LScriptObject& script) : ScriptableSpriteActor(script)
{
	script.load_property(_relative, "Relative");
	script.load_property(_additiveBlending, "AdditiveBlending");

	script.load_property(_emitter, "Emitter");
	script.load_property(_creator, "Creator");
	script.load_property(_generator, "Generator");
	script.load_property(_decorator, "Decorator");

	assert(_emitter);
	assert(_creator);
	assert(_generator);
}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::Draw(float x, float y)
{
	if (_container.empty())
		return;

	auto color_guard = Context<Color, Graphic::Image::Settings::ColorMultiplier>::CreateEmptyGuard();
	if (geometry.color != MX::Color::white())
		color_guard.Reset(geometry.color);

	//TODO fix me - code duplication
	if (relative())
	{
		MVP::Push();
		MVP::rotateZoom({x,y}, geometry.scale, geometry.angle);
	}
	else
	{
		x = x - geometry.position.x;
		y = y - geometry.position.y;
	}

	if (_additiveBlending)
	{
		auto g = Graphic::Blender::defaultAdditive().Use();
		_container.Draw(x, y);
			
	}
	else
	{
		_container.Draw(x, y);	
	}


	if (relative())
	{
		MVP::Pop();
	}

}

//TODO copypaste
void ParticleSystem::DrawCustom(float x, float y)
{
	if (_container.empty())
		return;

	auto color_guard = Context<Color, Graphic::Image::Settings::ColorMultiplier>::CreateEmptyGuard();
	if (geometry.color != MX::Color::white())
		color_guard.Reset(geometry.color);

	//TODO fix me - code duplication
	if (relative())
	{
		MVP::Push();
		MVP::rotateZoom({x,y}, geometry.scale, geometry.angle);
	}
	else
	{
		x = x - geometry.position.x;
		y = y - geometry.position.y;
	}

	if (_additiveBlending)
	{
		auto g = Graphic::Blender::defaultAdditive().Use();
		_container.DrawCustom(x, y);

	}
	else
	{
		_container.DrawCustom(x, y);
	}


	if (relative())
	{
		MVP::Pop();
	}

}

void ParticleSystem::Run()
{
	if (_decorator)
		_container.Run(this, _decorator);
	else
		_container.Run();
	ScriptableSpriteActor::Run();
	if (!_started)
	{
		_oldPosition = geometry.position;
		_started = true;
	}

	if (_active)
	{
		auto guard = Context<ParticleSystem>::Lock(this);
		_generator->OnBeforeGeneration();

		float time_percent = 1.0f;
		while (_generator->generate(time_percent))
		{
			std::unique_ptr<Particle> particle;
			float elapsed_time = (1.0f - time_percent) * Time::Timer::current().elapsed_seconds();
			{
				MX::Time::FakeTimer timer(elapsed_time, Time::Timer::current().total_seconds() - elapsed_time);
				auto guard = Context<Time::Timer>::Lock(timer);

				particle = _creator->CreateParticle();
				if (!relative())
					particle->geometry.position = lerp(_oldPosition, geometry.position, time_percent);

				_emitter->EmitParticle(*particle, *this);
			}
			particle->Run();
			_container.AddParticle(std::move(particle));

		}
		_generator->OnAfterGeneration();
	}

	

	_oldPosition = geometry.position;

	if (_container.empty() && (!_generator->IsActive() || !_active))
		Unlink();
}
	





void ParticleInit::Init()
{
	ScriptClassParser::AddCreator(L"Particles.SimpleSystem", new OutsideScriptClassCreatorContructor<ParticleSystem>());


	ParticleEmitterInit::Init();
	ParticleCreatorInit::Init();

	ScriptClassParser::AddCreator(L"Particles.Generator.Simple", new DefaultClassCreatorContructor<ParticleGeneratorSimple>()); 
	ScriptClassParser::AddCreator(L"Particles.Generator.AllAtOnce", new DefaultClassCreatorContructor<ParticleGeneratorAllAtOnce>());
}

#ifndef MXPARTICLEDECORATOR
#define MXPARTICLEDECORATOR

#include "Script/MXScriptObject.h"
#include "Utils/MXTime.h"
#include "MXParticleEmitter.h"
#include "Utils/MXUtils.h"

namespace MX{
namespace Graphic{

	class ParticleSystem;

	class ParticleDecorator : public ScriptObject
	{
	public:
		using pointer = std::shared_ptr<ParticleDecorator>;

		ParticleDecorator(LScriptObject& script)
		{

		}
		ParticleDecorator(const ParticleDecorator& shadow)
		{

		}

		virtual ~ParticleDecorator() {}

		virtual void DecorateParticle(Particle &p, ParticleSystem &s, bool alive = true)
		{

		}

		virtual bool isStateless() { return true; }
		virtual pointer clone()
		{
			return MX::make_shared<ParticleDecorator>(*this);
		}
	};


}
}

#endif

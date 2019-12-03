#ifndef MXPARTICLEDECORATOR
#define MXPARTICLEDECORATOR

#include "ParticleEmitter.h"
#include "script/ScriptObject.h"
#include "utils/Time.h"
#include "utils/Utils.h"

namespace MX
{
namespace Graphic
{

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

        virtual ~ParticleDecorator() { }

        virtual void DecorateParticle(Particle& p, ParticleSystem& s, bool alive = true)
        {
        }

        virtual bool isStateless() { return true; }
        virtual pointer clone()
        {
            return std::make_shared<ParticleDecorator>(*this);
        }
    };

}
}

#endif

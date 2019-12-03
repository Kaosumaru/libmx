#ifndef MXPARTICLEGENERATOR
#define MXPARTICLEGENERATOR
#include "ParticleEmitter.h"
#include "graphic/images/Image.h"
#include "scene/sprites/SpriteScene.h"
#include "script/ScriptObject.h"
#include "utils/Time.h"
#include "utils/Utils.h"

namespace MX
{
namespace Graphic
{

    class ParticleGenerator : public ScriptObjectString, public shared_ptr_init<ParticleGenerator>
    {
    public:
        ParticleGenerator() { }
        ParticleGenerator(const std::string& objectName)
            : ScriptObjectString(objectName)
        {
        }
        virtual ParticleGenerator::pointer clone() = 0;

        virtual void OnBeforeGeneration() {};
        virtual void OnAfterGeneration() {};

        //if this returns true, we should generate particle in time point last_time + elapsed_time * time_percent
        virtual bool generate(float& time_percent) = 0;
        virtual bool IsActive() { return true; }

    protected:
    };

    class ParticleGeneratorSimple : public ParticleGenerator
    {
    public:
        ParticleGeneratorSimple(float perSecond)
            : _particle_time(1.0f / perSecond)
        {
            _time = 0.0f;
        }

        ParticleGeneratorSimple(const std::string& objectName)
            : ParticleGenerator(objectName)
        {
            float perSecond = 1.0f;
            load_property(perSecond, "PerSecond");

            _particle_time = 1.0f / perSecond;
            _time = 0.0f;
        }

        ParticleGenerator::pointer clone() override
        {
            return std::make_shared<ParticleGeneratorSimple>(*this);
        }

        void OnBeforeGeneration() override
        {
            _time += Time::Timer::current().elapsed_seconds();
        }

        bool generate(float& time_percent) override
        {

            if (_time >= _particle_time)
            {
                _time -= _particle_time;
                float elapsed = Time::Timer::current().elapsed_seconds();
                time_percent = 1.0f - _time / elapsed;
                if (time_percent > 1.0f)
                {
                    assert(false);
                    time_percent = 1.0f;
                }
                else if (time_percent < 0.0f)
                {
                    assert(false);
                    time_percent = 0.0f;
                }

                return true;
            }

            return false;
        }

    protected:
        float _particle_time;
        float _time = 0.0f;
    };

    class ParticleGeneratorAllAtOnce : public ParticleGenerator
    {
    public:
        ParticleGeneratorAllAtOnce(const std::string& objectName)
            : ParticleGenerator(objectName)
        {
            load_property(_particleRange, "ParticleNumber");
            _particleNumber = Random::randomRange(_particleRange);
        }
        ParticleGenerator::pointer clone() override
        {
            _particleNumber = Random::randomRange(_particleRange);
            return std::make_shared<ParticleGeneratorAllAtOnce>(*this);
        }

        bool generate(float& time_percent) override
        {
            if (_particleNumber > 0)
            {
                _particleNumber--;
                time_percent = 1.0f;
                return true;
            }
            _hasgenerated = true;
            return false;
        }

        bool IsActive() override
        {
            return !_hasgenerated;
        }

    protected:
        int _particleNumber = 0;
        std::pair<int, int> _particleRange;
        bool _hasgenerated = false;
    };

}
}

#endif

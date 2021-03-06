#pragma once

#include "graphic/images/Image.h"
#include "scene/sprites/ScriptableSpriteActor.h"
#include "scene/sprites/SpriteScene.h"
#include "script/ScriptObject.h"
#include "utils/Time.h"

namespace MX
{
namespace Graphic
{

    class Particle;
    class ParticleEmitter;
    class ParticleCreator;
    class ParticleGenerator;
    class ParticleDecorator;
    class ParticleSystem;

    class ParticleContainer
    {
    public:
        ~ParticleContainer();
        void Run(ParticleSystem* system, const std::shared_ptr<ParticleDecorator>& decorator);
        void Run();
        void Draw(float x, float y);
        void DrawCustom(float x, float y);
        void RunAndDraw(float x, float y);

        bool empty() { return _particles.empty(); }

        void AddParticle(std::unique_ptr<Particle>&& particle);

    protected:
        std::list<std::unique_ptr<Particle>> _particles;
    };

    class ParticleSystem : public ScriptableSpriteActor, public ScopeSingleton<ParticleSystem>
    {
    public:
        ParticleSystem(LScriptObject& script);
        ParticleSystem(const ParticleSystem& other);
        ~ParticleSystem();

        void Draw(float x, float y) override;
        void DrawCustom(float x, float y) override;
        void Run() override;

        void StopThenUnlink() override { SetActive(false); }

        std::shared_ptr<ScriptableSpriteActor> cloneSprite() override { return std::make_shared<ParticleSystem>(*this); }

        void SetActive(bool active) { _active = active; }
        bool relative() { return _relative; }

    protected:
        bool _additiveBlending = false;
        bool _started = false;
        bool _active = true;
        bool _relative = false;

        glm::vec2 _oldPosition;

        std::shared_ptr<ParticleCreator> _creator;
        std::shared_ptr<ParticleEmitter> _emitter;
        std::shared_ptr<ParticleGenerator> _generator;
        std::shared_ptr<ParticleDecorator> _decorator;
        ParticleContainer _container;
    };

    class ParticleInit
    {
    public:
        static void Init();
    };

}
}

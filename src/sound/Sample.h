#pragma once
#include "utils/SignalizingVariable.h"
#include "utils/Singleton.h"
#include "utils/Utils.h"
#include <vector>

struct Mix_Chunk;

namespace SoLoud
{
class AudioSource;
class Soloud;
}

namespace MX
{
namespace Sound
{

    class SoundSettings : public ScopeSingletonDefault<SoundSettings>
    {
    public:
        SoundSettings();

        SignalizingVariable<float> baseVolume = 1.0f;
        float basePlaybackSpeed = 1.0f;
    };

    class Sample : public shared_ptr_init<Sample>, virtual public disable_copy_constructors
    {
    public:
        class Instance : public shared_ptr_init<Instance>, virtual public disable_copy_constructors
        {
            friend class Sample;

        public:
            Instance(const std::shared_ptr<Sample>& sample, int channel);
            static pointer Create(const std::shared_ptr<Sample>& sample, int channel);
            virtual ~Instance();
            void Stop();

        protected:
            int _channel = -1;
            std::shared_ptr<Sample> _sample;
        };

        Sample(const char* path);
        Sample(const std::string& path);
        virtual ~Sample();
        void Close();

        static pointer Create(const char* path);

        static void StopAll();
        static void ReserveSamples(unsigned samples);
        static void Shutdown();

        void Play(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);
        void PlayLooped(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);
        Instance::pointer PlayInstance(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);
        Instance::pointer PlayLoopedInstance(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);

        bool empty();
        void SetDefaultGain(float gain);

        float duration() { return _duration; }
        void SetPriority(int priority) { _priority = priority; }
        static SoLoud::Soloud& soLoud();

    protected:
        int _playSample(SoLoud::AudioSource* sound, float gain, float pan, float speed, bool looped, int priority);

        std::shared_ptr<SoLoud::AudioSource> _chunk = nullptr;
        int _priority = 4;
        float _duration = -1.0f;
        float _defaultGain = 1.0f;
        static int _mixer;
    };

    class RandomSample : public shared_ptr_init<RandomSample>
    {
    public:
        void AddSample(const Sample::pointer& sample);
        void Play(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);
        const Sample::pointer& Get();

    protected:
        std::vector<Sample::pointer> _samples;
    };

}
}

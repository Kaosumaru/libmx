#pragma once
#include "utils/SignalizingVariable.h"
#include "utils/Time.h"
#include "utils/Utils.h"

namespace SoLoud
{
class WavStream;
}

namespace MX
{
namespace Sound
{

    class StreamBase
    {
    public:
        virtual ~StreamBase() { }

        virtual void Close() = 0;

        virtual void Rewind() = 0;
        virtual void Play() = 0;
        virtual void Stop() = 0;
        virtual void SetSpeed(float speed) = 0;
        virtual void SetGain(float gain) = 0;
        virtual void SetPan(float pan) = 0;
        virtual void SetLooped(bool looped) = 0;

        virtual bool empty() = 0;

        MX::Signal<void(void)> onPlayedUpToEnd;

    protected:
        float _speed = 1.0f;
        float _pan = 0.0f;
        float _gain = 1.0f;
        bool _looped = true;
    };

    class Stream : public StreamBase, virtual public disable_copy_constructors
    {
    public:
        Stream(const char* path);
        virtual ~Stream();
        static std::shared_ptr<Stream> Create(const char* path);

        void Close() override;

        void Rewind() override;
        void Play() override;
        void Stop() override;
        void SetSpeed(float speed) override;
        void SetGain(float gain) override;
        void SetPan(float pan) override;
        void SetLooped(bool looped) override;

        bool empty() override;

        static void CloseAll();

    protected:
        std::shared_ptr<SoLoud::WavStream> _stream = nullptr;
        int _channel = -1;
    };

    class StreamManager : public Singleton<StreamManager>, public MX::SignalTrackable
    {
    public:
        StreamManager();

        void SetCurrent(const std::shared_ptr<StreamBase>& stream);
        void Run();

        void Deinit();

        void SetChangeTime(float time);
        void SetDefaultVolume(float volume);

        SignalizingVariable<float> volume = 1.0f;

    protected:
        void OnGainChanged(float gain);

        bool _crossfading = false;
        float _changeTime = 2.0f;

        Time::ManualStopWatch _stopWatch;
        std::shared_ptr<StreamBase> _oldStream;
        std::shared_ptr<StreamBase> _currentStream;
    };

}
}

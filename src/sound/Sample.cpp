#include "Sample.h"
#include <memory>
#include "Stream.h"
#include "utils/Random.h"

using namespace std;
using namespace MX;
using namespace MX::Sound;


class SampleAllGatherer : public Singleton<SampleAllGatherer>
{
public:
	void AddSample(Sample *sample)
	{
		_samples.insert(sample);
	}


	void RemoveSample(Sample *sample)
	{
		_samples.erase(sample);
	}

	void CloseAll()
	{
		for (auto &sample : _samples)
			sample->Close();
	}


	std::set<Sample*> _samples;
};

#ifdef SDLAUDIO
int Sample::_playSample(FMOD_SOUND *sound, float gain, float pan, float speed, bool looped, int priority)
{
	FMOD_CHANNEL *channel;
	if (FMOD_OK == FMOD_System_PlaySound(Sample::system() , sound, 0, true, &channel))
	{
		float rate = 44100.0f;
		FMOD_Channel_SetPriority(channel, priority);
		FMOD_Channel_SetVolume(channel, gain * SoundSettings::current().baseVolume);
		FMOD_Channel_SetFrequency(channel, rate * speed * SoundSettings::current().basePlaybackSpeed);
		FMOD_Channel_SetPan(channel, pan);

		if (looped)
		{
			FMOD_Channel_SetMode(channel, FMOD_LOOP_NORMAL);
			FMOD_Channel_SetLoopCount(channel, -1);
		}
		else
		{
			FMOD_Channel_SetMode(channel, FMOD_LOOP_OFF);
			FMOD_Channel_SetLoopCount(channel, 0);
		}
		
		FMOD_Channel_SetPaused(channel, false);
	}
	return channel;
}
#endif


SoundSettings::SoundSettings()
{

}


Sample::Instance::~Instance()
{
	Stop();
}
void Sample::Instance::Stop()
{
#ifdef SDLAUDIO
	FMOD_Channel_Stop(_channel);
	_channel = nullptr;
#endif
}

Sample::Instance::Instance(const std::shared_ptr<Sample> & sample)
{
	_sample = sample;
}

Sample::Instance::pointer Sample::Instance::Create(const std::shared_ptr<Sample> & sample)
{
	return std::make_shared<Sample::Instance>(sample);
}

Sample::Sample(const std::string &path) : Sample(path.c_str())
{

}

Sample::Sample(const char *path)
{
#ifdef SDLAUDIO
	FMOD_System_CreateSound(_fmodSystem, path, FMOD_DEFAULT, NULL, &_sample);
	_defaultGain = 1.0f;
	if (_sample)
	{
		EstimateDuration();
		SampleAllGatherer::get().AddSample(this);
	}
#endif	
}

void Sample::EstimateDuration()
{
#ifdef SDLAUDIO
	unsigned int length;
	if (FMOD_OK == FMOD_Sound_GetLength(_sample, &length, FMOD_TIMEUNIT_MS))
		_duration = (float)length / 1000.0f;
#endif
}

Sample::~Sample()
{
#ifdef SDLAUDIO
	if (_sample)
		SampleAllGatherer::get().RemoveSample(this);
#endif
	Close();
}

void Sample::Close()
{
#ifdef SDLAUDIO
	if (_sample)
		FMOD_Sound_Release(_sample);
	_sample = nullptr;
#endif
}

Sample::pointer Sample::Create(const char *path)
{
	auto bit = std::make_shared<Sample>(path);
#ifndef NDEBUG
	if (bit->empty())
		return nullptr;
#endif
	return bit;
}

bool Sample::empty()
{
#ifdef SDLAUDIO
	return _sample == nullptr;
#else
	return true;
#endif
}

void Sample::Update()
{
#ifdef SDLAUDIO
	FMOD_System_Update(_fmodSystem);
#endif
}

void Sample::Shutdown()
{
	SampleAllGatherer::get().CloseAll();
	Stream::CloseAll();
#ifdef SDLAUDIO
	FMOD_System_Release(_fmodSystem);
#endif
}


void Sample::SetDefaultGain(float gain)
{
	_defaultGain = gain;
}

void Sample::ReserveSamples(unsigned samples)
{
#ifdef SDLAUDIO
	FMOD_System_Create(&_fmodSystem);
	FMOD_System_Init(_fmodSystem, samples, FMOD_INIT_NORMAL, NULL);
#endif
}
void Sample::Play(float gain , float pan, float speed)
{
#ifdef SDLAUDIO
	_playSample(_sample, gain * _defaultGain, pan, speed, false, _priority);
#endif
}
void Sample::PlayLooped(float gain, float pan, float speed)
{
#ifdef SDLAUDIO
	_playSample(_sample, gain * _defaultGain, pan, speed, true, _priority);
#endif
}
Sample::Instance::pointer Sample::PlayInstance(float gain , float pan, float speed)
{
#ifdef SDLAUDIO
	return Sample::Instance::Create(shared_from_this(), _playSample(_sample, gain * _defaultGain, pan, speed, false, _priority));
#else
	return nullptr;
#endif
}
Sample::Instance::pointer Sample::PlayLoopedInstance(float gain , float pan, float speed)
{
#ifdef SDLAUDIO
	return Sample::Instance::Create(shared_from_this(), _playSample(_sample, gain * _defaultGain, pan, speed, true, _priority));
#else
	return nullptr;
#endif
}
void Sample::StopAll()
{
	assert(false);
}

void RandomSample::AddSample(const Sample::pointer &sample)
{
	_samples.push_back(sample);
}
void RandomSample::Play(float gain, float pan, float speed)
{
	Get()->Play(gain, pan, speed);
}
const Sample::pointer &RandomSample::Get()
{
	return Random::randomFrom(_samples);
}
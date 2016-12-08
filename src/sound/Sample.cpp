#include "Sample.h"
#include <memory>
#include "Stream.h"
#include "utils/Random.h"
#include "SDL_mixer.h"

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


int Sample::_playSample(Mix_Chunk *sound, float gain, float pan, float speed, bool looped, int priority)
{
	auto channel = Mix_PlayChannel(-1, sound, looped ? -1 : 0);
	Mix_Volume(channel, 128 * gain);

#ifdef SDLAUDIO
	Mix_SetPanning(channel, left, 255 - left);
	FMOD_Channel_SetFrequency(channel, rate * speed * SoundSettings::current().basePlaybackSpeed);
#endif
	return channel;
}



SoundSettings::SoundSettings()
{

}


Sample::Instance::~Instance()
{
	Stop();
}
void Sample::Instance::Stop()
{
	if (_channel == -1)
		return;
	Mix_HaltChannel(_channel);
	_channel = -1;
}

Sample::Instance::Instance(const std::shared_ptr<Sample> & sample, int channel)
{
	_channel = channel;
	_sample = sample;
}

Sample::Instance::pointer Sample::Instance::Create(const std::shared_ptr<Sample> & sample, int channel)
{
	return std::make_shared<Sample::Instance>(sample, channel);
}

Sample::Sample(const std::string &path) : Sample(path.c_str())
{

}

Sample::Sample(const char *path)
{
	_chunk = Mix_LoadWAV(path);
	if (_chunk)
	{
		EstimateDuration();
		SampleAllGatherer::get().AddSample(this);
	}
}

Uint32 getChunkTimeMilliseconds(Mix_Chunk *chunk)
{
	Uint32 points = 0;
	Uint32 frames = 0;
	int freq = 0;
	Uint16 fmt = 0;
	int chans = 0;
	/* Chunks are converted to audio device format... */
	if (!Mix_QuerySpec(&freq, &fmt, &chans))
		return 0; /* never called Mix_OpenAudio()?! */

				  /* bytes / samplesize == sample points */
	points = (chunk->alen / ((fmt & 0xFF) / 8));

	/* sample points / channels == sample frames */
	frames = (points / chans);

	/* (sample frames * 1000) / frequency == play length in ms */
	return (frames * 1000) / freq;
}

void Sample::EstimateDuration()
{
	_duration = getChunkTimeMilliseconds(_chunk);
}

Sample::~Sample()
{
	if (_chunk)
		SampleAllGatherer::get().RemoveSample(this);
	Close();
}

void Sample::Close()
{
	if (_chunk)
		Mix_FreeChunk(_chunk);
	_chunk = nullptr;
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
	return _chunk == nullptr;
}

void Sample::Shutdown()
{
	SampleAllGatherer::get().CloseAll();
	Stream::CloseAll();
	Mix_CloseAudio();
	Mix_Quit();
}


void Sample::SetDefaultGain(float gain)
{
	_defaultGain = gain;
}

int Sample::_mixer = 0;

void Sample::ReserveSamples(unsigned samples)
{
	int flags=MIX_INIT_OGG;
	int initted=Mix_Init(flags);
	_mixer = Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	//WIPLOG
}
void Sample::Play(float gain , float pan, float speed)
{
	_playSample(_chunk, gain * _defaultGain, pan, speed, false, _priority);
}
void Sample::PlayLooped(float gain, float pan, float speed)
{
	_playSample(_chunk, gain * _defaultGain, pan, speed, true, _priority);
}
Sample::Instance::pointer Sample::PlayInstance(float gain , float pan, float speed)
{
	return Sample::Instance::Create(shared_from_this(), _playSample(_chunk, gain * _defaultGain, pan, speed, false, _priority));
}
Sample::Instance::pointer Sample::PlayLoopedInstance(float gain , float pan, float speed)
{
	return Sample::Instance::Create(shared_from_this(), _playSample(_chunk, gain * _defaultGain, pan, speed, true, _priority));
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
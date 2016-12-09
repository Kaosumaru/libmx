#include "Sample.h"
#include <memory>
#include <iostream>
#include "Stream.h"
#include "utils/Random.h"
#include "soloud.h"
#include "soloud_wav.h"

using namespace std;
using namespace MX;
using namespace MX::Sound;


SoLoud::Soloud soloud;

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


int Sample::_playSample(SoLoud::AudioSource *sound, float gain, float pan, float speed, bool looped, int priority)
{
    auto channel = soloud.play( *sound, gain, pan );
    soloud.setRelativePlaySpeed( channel, speed );
    soloud.setLooping( channel, looped );
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
	soloud.stop(_channel);
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
    auto wav = std::make_shared<SoLoud::Wav>();
    auto res = wav->load(path);
    _chunk = std::move( wav );
    std::cout << "Sample " << path << " -> " << res << "\n";
	if (_chunk)
	{
		EstimateDuration();
		SampleAllGatherer::get().AddSample(this);
	}
}


void Sample::EstimateDuration()
{
	//_duration = 1000;
}

Sample::~Sample()
{
	if (_chunk)
		SampleAllGatherer::get().RemoveSample(this);
	Close();
}

void Sample::Close()
{
	_chunk.reset();
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
    soloud.deinit();
}


void Sample::SetDefaultGain(float gain)
{
	_defaultGain = gain;
}

int Sample::_mixer = 0;

void Sample::ReserveSamples(unsigned samples)
{
    soloud.init();
	//WIPLOG
}
void Sample::Play(float gain , float pan, float speed)
{
	_playSample(_chunk.get(), gain * _defaultGain, pan, speed, false, _priority);
}
void Sample::PlayLooped(float gain, float pan, float speed)
{
	_playSample(_chunk.get(), gain * _defaultGain, pan, speed, true, _priority);
}
Sample::Instance::pointer Sample::PlayInstance(float gain , float pan, float speed)
{
	return Sample::Instance::Create(shared_from_this(), _playSample(_chunk.get(), gain * _defaultGain, pan, speed, false, _priority));
}
Sample::Instance::pointer Sample::PlayLoopedInstance(float gain , float pan, float speed)
{
	return Sample::Instance::Create(shared_from_this(), _playSample(_chunk.get(), gain * _defaultGain, pan, speed, true, _priority));
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
#pragma once
#include <vector>
#include "utils/Utils.h"
#include "utils/SignalizingVariable.h"
#include "utils/Singleton.h"

namespace MX{
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
		Instance(const std::shared_ptr<Sample> & sample);
		static pointer Create(const std::shared_ptr<Sample> & sample);
		virtual ~Instance();
		void Stop();
	protected:
		std::shared_ptr<Sample> _sample;
	};


	Sample(const char *path);
	Sample(const std::string &path);
	virtual ~Sample();
	void Close();

	static pointer Create(const char *path);


	static void ReserveSamples(unsigned samples);
	static void Update();
	static void Shutdown();

	void Play(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);
	void PlayLooped(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);
	Instance::pointer PlayInstance(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);
	Instance::pointer PlayLoopedInstance(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);
	static void StopAll();


	bool empty();
	void SetDefaultGain(float gain);

	float duration() { return _duration; }
	void SetPriority(int priority) { _priority = priority; }
protected:
	void EstimateDuration();

	int _priority = 4;
	float _duration = -1.0f;
	float _defaultGain;
};

class RandomSample : public shared_ptr_init<RandomSample>
{
public:
	void AddSample(const Sample::pointer &sample);
	void Play(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f);
	const Sample::pointer &Get();
protected:
	std::vector<Sample::pointer> _samples;
};


}
}

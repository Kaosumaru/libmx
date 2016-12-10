#include "Stream.h"
#include <memory>
#include <set>
#include "Sample.h"
#include "soloud.h"
#include "soloud_wavstream.h"

using namespace MX;
using namespace MX::Sound;






class StreamAllGatherer : public Singleton<StreamAllGatherer>
{
public:
	void AddStream(Stream *stream)
	{
		_streams.insert(stream);
	}


	void RemoveStream(Stream *stream)
	{
		_streams.erase(stream);
	}

	void CloseAll()
	{
		for (auto &stream : _streams)
			stream->Close();
	}

#ifdef SDLAUDIO
	static FMOD_RESULT F_CALLBACK stream_callback(FMOD_CHANNELCONTROL *chanControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *commandData1, void *commandData2)
	{
		if (controlType != FMOD_CHANNELCONTROL_CHANNEL)
			return FMOD_OK;

		FMOD_CHANNEL *channel = (FMOD_CHANNEL *)chanControl;
		Stream *stream = nullptr;

		FMOD_Channel_GetUserData(channel, (void**)&stream);

		if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END)
		{
			stream->onPlayedUpToEnd();
			return FMOD_OK;
		}

		return FMOD_OK;
	}
#endif

	std::set<Stream*> _streams;
};

void Stream::CloseAll()
{
	StreamAllGatherer::get().CloseAll();
}

Stream::Stream(const char *path)
{
	_stream = std::make_shared<SoLoud::WavStream>();
	_stream->load(path);
	StreamAllGatherer::get().AddStream(this);
	//WIPLOG
}

Stream::~Stream()
{
	StreamAllGatherer::get().RemoveStream(this);
	Close();
}

void Stream::Close()
{
	Stop();
	_stream.reset();

}

std::shared_ptr<Stream> Stream::Create(const char *path)
{
	auto stream = std::make_shared<Stream>(path);
	if (stream->empty())
		return nullptr;
	return stream;
}

void Stream::Rewind()
{
	if (_channel != -1)
		Sample::soLoud().seek(_channel, 0);
}
void Stream::Play()
{
	_channel = Sample::soLoud().play( *_stream );
	Sample::soLoud().setProtectVoice(_channel, true);
	SetSpeed(_speed);
	SetGain(_gain);
	SetPan(_pan);

#ifdef SDLAUDIO
	if (_channel || !_sample)
		return;

	if (FMOD_OK == FMOD_System_PlaySound(Sample::system(), _sample, 0, true, &_channel))
	{
		float rate = 44100.0f;
		FMOD_Channel_SetPosition(_channel, 0, FMOD_TIMEUNIT_MS);
		FMOD_Channel_SetUserData(_channel, this);
		FMOD_Channel_SetCallback(_channel, &StreamAllGatherer::stream_callback);
		FMOD_Channel_SetVolume(_channel, _gain);
		FMOD_Channel_SetFrequency(_channel, rate * _speed);
		FMOD_Channel_SetPan(_channel, _pan);
		FMOD_Channel_SetPriority(_channel, 0);
		SetLooped(_looped);
		FMOD_Channel_SetPaused(_channel, false);
	}
#endif
}
void Stream::Stop()
{
	if (_channel != -1)
	{
		Sample::soLoud().setProtectVoice(_channel, false);
		Sample::soLoud().stop(_channel);
	}
	_channel = -1;
}
void Stream::SetSpeed(float speed)
{
	_speed = speed;
	float rate = 44100.0f;
	if (_channel != -1)
		Sample::soLoud().setRelativePlaySpeed(_channel, _speed);
}
void Stream::SetGain(float gain)
{
	_gain = gain;
	if (_channel != -1)
		Sample::soLoud().setVolume(_channel, _gain);
}
void Stream::SetPan(float pan)
{
	_pan = pan;
	if (_channel != -1)
		Sample::soLoud().setPan(_channel, _pan);
}
void Stream::SetLooped(bool looped)
{
	_looped = looped;
	_stream->setLooping(looped);
}

bool Stream::empty()
{
	return _stream != nullptr;
}



StreamManager::StreamManager()
{
	float vol = 0.0f;
#ifdef WIPSERIALIZE
	if (MX::Database::get().settings().try_get("StreamManager.Settings.Volume", vol))
		volume = vol;
#endif
	using namespace std::placeholders;
	volume.onValueChanged.connect(std::bind(&StreamManager::OnGainChanged, this, _1), this);
}

void StreamManager::SetDefaultVolume(float v)
{
	float vol = 0.0f;

#ifdef WIPSERIALIZE
	if (!MX::Database::get().settings().try_get("StreamManager.Settings.Volume", vol))
		volume = v;
#endif
}


void StreamManager::SetCurrent(const std::shared_ptr<StreamBase>& stream)
{
	if (_currentStream == stream)
		return;


	if (_oldStream)
	{
		if (_oldStream)
			_oldStream->Stop();
		_oldStream = nullptr;
	}

	if (_crossfading)
	{
		_crossfading = false;
		//we are crossfading now, so disable old streams altogether
		if (_currentStream)
			_currentStream->Stop();
		_oldStream = nullptr;
	}
	else
	{
		_stopWatch.Start(_changeTime);
		_crossfading = true;
		_oldStream = _currentStream;
	}

	_currentStream = stream;

	if (stream)
	{
		stream->SetGain(_crossfading ? 0.0f : volume.directValueAccess());
		stream->SetLooped(true);
		stream->Rewind();
		stream->Play();
	}

}

void StreamManager::Run()
{
	if (_crossfading)
	{
		if (_stopWatch.Tick())
		{
			if (_oldStream)
				_oldStream->Stop();
			_oldStream = nullptr;
			_crossfading = false;
		}
		if (_oldStream)
			_oldStream->SetGain(volume * _stopWatch.inverse_percent());
		if (_currentStream)
			_currentStream->SetGain(volume * _stopWatch.percent());
	}
}

void StreamManager::Deinit()
{
#ifdef SDLAUDIO
	MX::Database::get().settings().set("StreamManager.Settings.Volume", volume.directValueAccess());
#endif
	if (_oldStream)
		_oldStream->Stop();
	_oldStream = nullptr;
	if (_currentStream)
		_currentStream->Stop();
	_currentStream = nullptr;
}

void StreamManager::OnGainChanged(float gain)
{
	if (_currentStream && !_crossfading)
		_currentStream->SetGain(gain * _stopWatch.percent());

	
}

void StreamManager::SetChangeTime(float time)
{
	_changeTime = time;
}
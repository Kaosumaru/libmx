#include "ScriptSoundClass.h"
#include "game/resources/Paths.h"
#include "game/resources/Resources.h"
#include "script/PropertyLoaders.h"
#include "script/Script.h"
#include "utils/Log.h"
#include "utils/Random.h"

using namespace MX;

class RandomSpeedSample : public Sound::Sample
{
public:
    using Sound::Sample::Sample;

    void SetRandomSpeed(const std::pair<float, float>& speed)
    {
        _randomSpeed = speed;
    }

protected:
    int _playSample(SoLoud::AudioSource* sound, float gain, float pan, float speed, bool looped, int priority) override
    {
        float m = Random::randomRange(_randomSpeed);
        return Sound::Sample::_playSample(sound, gain, pan, speed * m, looped, priority);
    }

    std::pair<float, float> _randomSpeed = { 1.0f, 1.0f };
};

ScriptSoundClass::ScriptSoundClass()
{
}

const std::shared_ptr<Sound::Sample>& ScriptSoundClass::sound() const
{
    return _sound;
}

bool ScriptSoundClass::onParse()
{
    std::string path;
    load_property_child(path, "Path");

    std::pair<float, float> randomSpeed;
    float gain = 1.0f;

    if (load_property(randomSpeed, "RandomSpeed"))
    {
        auto sample = std::make_shared<RandomSpeedSample>(MX::Paths::get().pathToResource(path));
        if (!sample->empty())
        {
            sample->SetRandomSpeed(randomSpeed);
            _sound = sample;
        }
    }
    else
    {
        _sound = Resources::get().loadSound(path.c_str());
    }

    if (!_sound)
    {
        spdlog::error("Cannot load sound {} in {}", path, object());
    }
    if (!_sound)
        return true;

    if (load_property(gain, "Gain"))
        _sound->SetDefaultGain(gain);

    int priority = 0;
    if (load_property(priority, "Priority"))
        _sound->SetPriority(priority);

    return true;
}

ScriptRandomSound::ScriptRandomSound()
{
}

const std::shared_ptr<Sound::Sample>& ScriptRandomSound::sound() const
{
    static std::shared_ptr<Sound::Sample> dummy;
    if (_sounds.empty())
    {
        assert(false);
        return dummy;
    }

    return _sounds[rand() % _sounds.size()];
}

bool ScriptRandomSound::onParse()
{
    load_property(_sounds, "Sounds");

    float gain = 1.0f;
    bool gainl = load_property(gain, "Gain");

    int priority = 0;
    bool priorityl = load_property(priority, "Priority");

    unsigned framesOffset = 0, framesCount = 0;
    load_property(framesOffset, "Sounds_offset");
    load_property(framesCount, "Sounds_count");

    std::string filenameTemplate;
    load_property(filenameTemplate, "Filename_template");

    char tmp[256];
    for (unsigned i = framesOffset; i < framesOffset + framesCount; i++)
    {
        sprintf(tmp, filenameTemplate.c_str(), i);
        auto sound = Resources::get().loadSound(tmp);

        if (sound)
        {
            if (gainl)
                sound->SetDefaultGain(gain);
            if (priorityl)
                sound->SetPriority(priority);
            _sounds.push_back(sound);
        }
        else
        {
            spdlog::error("Cannot load sound {} in {}", tmp, object());
        }
    }

    if (_sounds.size() == 0)
    {
        spdlog::error("No sounds in {}", object());
    }

    return true;
}

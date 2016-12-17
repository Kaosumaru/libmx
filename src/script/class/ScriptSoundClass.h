#pragma once
#include <string>
#include "sound/Sample.h"
#include "utils/Utils.h"
#include "script/ScriptClass.h"


namespace MX{



class ScriptSoundClass : public ScriptClass
{
public:
	ScriptSoundClass();

	virtual const std::shared_ptr<Sound::Sample>& sound() const;
protected:
	bool onParse() override;

	std::shared_ptr<Sound::Sample> _sound;
};




class ScriptRandomSound : public ScriptSoundClass
{
public:
	ScriptRandomSound();

	const std::shared_ptr<Sound::Sample>& sound() const override;
protected:
	bool onParse() override;

	std::vector<std::shared_ptr<Sound::Sample>> _sounds;

	
};

}
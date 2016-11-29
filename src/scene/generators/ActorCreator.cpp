#include "ActorCreator.h"
#include "Script/ScriptClassParser.h"
#include "Script/ExtendedPropertyLoaders.h"

using namespace MX;
using namespace MX::Graphic;


SpecificActorCreator::SpecificActorCreator(LScriptObject& script) : ActorCreator(script)
{
	int total_sum = 0;
	auto &actors = script.property("Actors").array();
	for (auto &actor : actors)
	{
		using Pair = std::pair<Entry, int>;
		Pair pair;
		if (!PropertyLoader<Pair>::load(pair, actor))
		{
			auto actor_object = actor->to_object<ScriptableSpriteActor>();
			if (actor_object)
			{
				total_sum++;
				_actors.push_back(std::make_pair(actor_object, 1));
			}
			continue;
		}

		total_sum += pair.second;
		_actors.push_back(pair);
	}

	_remainingToGenerate = _maxActors = total_sum;
}

class SwitchActorCreator : public ActorCreator
{
public:

	SwitchActorCreator(LScriptObject& script) : ActorCreator(script)
	{
		script.load_property(_condition, "Condition");
		script.load_property_children(_entries, "Case");
		script.load_property(_defaultCase, "Default");
	}

	std::shared_ptr<ScriptableSpriteActor> onCreateActor() override
	{
		float cond = *_condition;

		auto it = _entries.find(cond);
		if (it == _entries.end())
		{
			return _defaultCase.CreateActor();
		}
		return it->second.CreateActor();
	}

	ActorCreator::pointer clone() override { return std::make_shared<SwitchActorCreator>(*this); }

	using Entry = ActorCreatorEntry;

	Scriptable::Value::pointer _condition;
	std::map<float, Entry> _entries;
	Entry                  _defaultCase;
};



class RandomActorCreator : public ActorCreator
{
protected:
	using RandomActor = RandomItem<ScriptableSpriteActor>;
public:
	RandomActorCreator()
	{
	}

	RandomActorCreator(LScriptObject& script) : ActorCreator(script)
	{
		script.load_property(_randomActors, "Actors");
	}

	std::shared_ptr<ScriptableSpriteActor> onCreateActor() override
	{
		return _randomActors.randomItem()->cloneSprite();
	}

	virtual ActorCreator::pointer clone() { return std::make_shared<RandomActorCreator>(*this); }

	RandomActor _randomActors;
};


void ActorCreatorInit::Init()
{
	ScriptClassParser::AddCreator(L"Scene.ActorFactory.Creator.Switch", new OutsideScriptClassCreatorContructor<SwitchActorCreator>());
	ScriptClassParser::AddCreator(L"Scene.ActorFactory.Creator.Random", new OutsideScriptClassCreatorContructor<RandomActorCreator>());
#ifdef WIP
	ScriptClassParser::AddCreator(L"Scene.ActorFactory.Creator.Random.NonLocked", new OutsideScriptClassCreatorContructor<RandomNonLockedActorCreator>());
#endif
	ScriptClassParser::AddCreator(L"Scene.ActorFactory.Creator.Specific", new OutsideScriptClassCreatorContructor<SpecificActorCreator>());
}

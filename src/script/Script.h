#pragma once
#include <unordered_map>
#include "utils/Singleton.h"
#include "scriptable/ScriptableValue.h"

#if SCRIPTWIP
#ifdef _DEBUG
#define MX_MT_IMPL
#endif
#endif

namespace MX{
struct KeyContext {};
class ScriptParser;

class Script : virtual public disable_copy_constructors, public Singleton<Script>
{
public:


	friend class ScriptParser;
	friend class Scriptable::Value;
	static const std::wstring localize(const std::string& label);
	static bool loadScript(const std::string& file);

	static const Scriptable::Value::pointer& propertyOrNull(const std::string& obj, const std::string& property);

	static const Scriptable::Value::pointer& object(const std::string& label);
	static const Scriptable::Value::pointer& objectOrNull(const std::string& label);
	static const Scriptable::Value& valueOf(const std::string& label);
	static bool valueExists(const std::string& label);
	static void Clear();
	static void ParseDirs(const std::list<std::string>& paths, bool reset = false);

	static const std::wstring parseString(const std::string& path, const std::wstring& text);

	static Signal<void(void)> onParsed;

	static void Deinit() { onParsed.disconnect_all_slots(); }


	const Scriptable::Value::pointer& SetObject(const std::string &key, const Scriptable::Value::Map& map = {});
	const Scriptable::Value::pointer& SetPair(const std::string &key, bool value);
	const Scriptable::Value::pointer& SetPair(const std::string &key, const std::wstring &value);
	const Scriptable::Value::pointer& SetPair(const std::string &key, float number, bool percent = false);
	const Scriptable::Value::pointer& SetPair(const std::string &key, const std::vector<Scriptable::Value::pointer>& members);
	const Scriptable::Value::pointer& SetPairFunctor(const std::string &key, const std::function<float()> &number_functor);
protected:


	bool _loadScript(const std::string& file);
	const Scriptable::Value& _valueOf(const std::string& label);
	bool _valueExists(const std::string& label);
	const Scriptable::Value::pointer& _object(const std::string& label);
	const Scriptable::Value::pointer& _objectOrNull(const std::string& label);
	const std::wstring _localize(const std::string& label);

	std::unordered_map<std::string, Scriptable::Value::pointer> _localized_values;
};



extern const std::wstring loc(const std::string& label);

}

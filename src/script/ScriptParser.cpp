#include "ScriptParser.h"
#include "Script.h"
#include "ScriptClassParser.h"

#include "deps/json/json.h"
#include "msl/MSL.h"
#include "utils/ListFiles.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

using namespace MX;

class MSLScriptParser : public ScriptParser
{
public:
    MSLScriptParser(const std::string& path, Script& script, bool is_data = false)
        : ScriptParser(script)
        , _file_path(path)
        , _is_data(is_data)
    {
    }
    bool Parse()
    {

        std::string input;

        if (!_is_data)
        {
            std::ifstream instream(_file_path);

            if (instream.fail())
                return false;

            input = { (std::istreambuf_iterator<char>(instream)),
                std::istreambuf_iterator<char>() };
        }
        else
        {
            input = _file_path;
        }

        auto root = msl::Value::fromString(input);
        if (!root)
        {
            assert(false);
            return false;
        }

        Parse(root, "", 0);

        return true;
    }

    const Scriptable::Value::pointer& Parse(const msl::Value::pointer& value, const std::string& path, int level)
    {
        switch (value->type())
        {
        case msl::Value::Type::Percent:
            return _Script.SetPair(path, (float)value->asFloat(), true);

        case msl::Value::Type::Float:
            return _Script.SetPair(path, (float)value->asFloat());

        case msl::Value::Type::String:
        {
            static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
            return _Script.SetPair(path, convert.from_bytes(value->asString()));
        }

        case msl::Value::Type::Boolean:
            return _Script.SetPair(path, value->asBool());

        case msl::Value::Type::Map:
            return ParseMap(value, path, level);

        case msl::Value::Type::Array:
            return ParseArray(value, path, level);

        case msl::Value::Type::Null:
            return ParseNull(value, path, level);

        default:
            assert(false); //NYI
            break;
        };

        static Scriptable::Value::pointer ptr;
        return ptr;
    }

    const Scriptable::Value::pointer& ParseNull(const msl::Value::pointer& null_value, const std::string& path, int level)
    {
        ParseNamed(null_value, path, level);
        return _Script.SetObject(path);
    }

    const Scriptable::Value::pointer& ParseMap(const msl::Value::pointer& map_value, const std::string& path, int level)
    {
        ParseNamed(map_value, path, level);
        auto& map = map_value->asMap();
        auto& obj = _ParseMapImpl(map, path, level);

#ifndef _DEBUG
        //TODO check this
        if (Script::valueExists(path + ".Class"))
            return ScriptClassParser::Parse(path);
#endif
        return obj;
    }

    const Scriptable::Value::pointer& ParseArray(const msl::Value::pointer& array_value, const std::string& path, int level)
    {
        //TODO test named array inside named array

        auto children_path = path;
        bool named = false;

        if (ParseNamed(array_value, path, level))
        {
            named = true;
            children_path += ".Children";
        }

        auto& array = array_value->asArray();
        std::vector<Scriptable::Value::pointer> _members;
        int i = 0;
        for (auto& value : array)
        {
            auto member = Parse(value, children_path + "." + std::to_string(i), ++level);
            if (member)
                _members.push_back(member), i++;
        }

        if (named)
        {
            _Script.SetPair(children_path, _members);
            return _Script.SetObject(path);
        }
        return _Script.SetPair(children_path, _members);
    }

protected:
    bool ParseNamed(const msl::Value::pointer& map_value, const std::string& path, int level)
    {
        auto& name = map_value->name();
        if (name.empty())
            return false;

        static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

        auto propertyPath = path;
        if (name[0] == '&')
            propertyPath += ".Prototype";
        else
            propertyPath += ".Object";

        _Script.SetPair(propertyPath, convert.from_bytes(name));
        _ParseMapImpl(map_value->attributes(), path, level);
        return true;
    }

    const Scriptable::Value::pointer& _ParseMapImpl(const msl::Value::MapType& map, const std::string& p, int level)
    {
        std::string pathCopy;
        bool useMap = false;
        if (!p.empty() && p.back() == '!')
        {
            useMap = true;
            pathCopy = p;
            pathCopy.pop_back();
        }
        auto& path = useMap ? pathCopy : p;

        Scriptable::Value::Map members;

        for (auto& pair : map)
        {
            auto& key = pair.first;
            auto& value = pair.second;

            std::string value_path;
            if (level == 0)
                value_path = key->asString();
            else
                value_path = path + "." + key->asString();

#ifdef _DEBUG
            if (value->type() == msl::Value::Type::String && key->asString() == "Class")
            {
                value_path = path + ".Object";
            }
#endif

            auto v = Parse(value, value_path, level + 1);

            if (useMap)
                members[key->asString()] = v;
        }

        return _Script.SetObject(path, members);
    }

    std::string _file_path;
    bool _is_data = false;
};

class JSONScriptParser : public ScriptParser
{
    char* _buffer;

public:
    JSONScriptParser(char* buffer, Script& script, const char* fileName)
        : ScriptParser(script)
        , _buffer(buffer)
    {
        _fileName = fileName;
    }
    bool Parse()
    {
        char* errorPos = 0;
        char* errorDesc = 0;
        int errorLine = 0;
        block_allocator allocator(1 << 10);

        json_value* root = json_parse(_buffer, &errorPos, &errorDesc, &errorLine, &allocator);

        if (root)
        {
            Parse(root, "", -1);
            return true;
        }

        if (errorPos || errorDesc)
        {
            std::stringstream ss;
            ss << "Error in parsing file '" << _fileName << "'" << std::endl;
            ss << "Pos: " << errorPos << " line: " << errorLine << " " << std::endl;
            ss << errorDesc;

            std::cout << "---------------------" << std::endl;
            std::cout << ss.str() << std::endl;
            std::cout << "---------------------" << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;

            assert(false);
        }

        return false;
    }

protected:
    const char* _fileName;

    const Scriptable::Value::pointer& Parse(json_value* value, const std::string& path, int index = 0)
    {

        std::stringstream ss;
        ss << path;

#ifdef _DEBUG
        if (value->type == JSON_STRING && value->name && strcmp(value->name, "Class") == 0)
        {
            static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
            _Script.SetPair(ss.str() + "Object", convert.from_bytes(value->string_value));
        }
#endif

        if (value->name)
            ss << value->name;
        else if (index != -1)
            ss << index;
        switch (value->type)
        {
        case JSON_nullptr:
            break;
        case JSON_ARRAY:
        {
            std::string new_path = ss.str();
            if (index != -1)
                new_path += ".";

            std::vector<Scriptable::Value::pointer> _members;
            int i = 0;
            for (json_value* it = value->first_child; it; it = it->next_sibling)
            {
                auto member = Parse(it, new_path, i);
                if (member)
                    _members.push_back(member), i++;
            }

            return _Script.SetPair(ss.str(), _members);
        }
        break;
        case JSON_OBJECT:
        {
            std::string new_path = ss.str();
            if (index != -1)
                new_path += ".";

            for (json_value* it = value->first_child; it; it = it->next_sibling)
            {
                Parse(it, new_path);
            }

#ifndef _DEBUG
            if (Script::valueExists(new_path + "Class"))
                return ScriptClassParser::Parse(ss.str());
#endif
            return _Script.SetObject(ss.str());
        }
        break;
        case JSON_BOOL:
        {

            return _Script.SetPair(ss.str(), value->int_value != 0);
        }

        case JSON_FLOAT:
        case JSON_INT:
        {
            long double number = (value->type == JSON_INT) ? (float)value->int_value : value->float_value;

            return _Script.SetPair(ss.str(), (float)number);
        }
        break;

        case JSON_STRING:
            if (value->string_value)
            {
                static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
                return _Script.SetPair(ss.str(), convert.from_bytes(value->string_value));
            }
            break;
        }
        static Scriptable::Value::pointer dummy;
        return dummy;
    }
};

namespace impl
{
void ScriptParser_JSONDoParse(const std::string& path, Script& sc)
{
    FILE* fp = fopen(path.c_str(), "rb");
    if (fp)
    {

        fseek(fp, 0, SEEK_END);
        int size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        std::vector<char> buffer(size + 1);
        fread(&buffer[0], 1, size, fp);
        fclose(fp);

        JSONScriptParser Script(buffer.data(), sc, path.c_str());
        Script.Parse();
    }
}

void ScriptParser_MSLDoParse(const std::string& path, Script& sc)
{
    MSLScriptParser Script(path, sc);
    Script.Parse();
}

void ScriptParser_DoParse(const std::string& path, Script& sc)
{
    if (FileExtension(path) == "json")
        ScriptParser_JSONDoParse(path, sc);
    else if (FileExtension(path) == "msl")
        ScriptParser_MSLDoParse(path, sc);
}
};

ScriptParser::ScriptParser(Script& Script)
    : _Script(Script)
{
}

void ScriptParser::ParseStringToScript(const std::string& data, Script& sc)
{
    MSLScriptParser Script(data, sc, true);
    Script.Parse();
}

void ScriptParser::ParseFileToScript(const std::string& path, Script& mainSC)
{
#ifdef MX_MT_IMPL
    Script sc;
    impl::ScriptParser_DoParse(path, sc);

    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    auto& other_values = sc._localized_values;

    for (auto& pair : other_values)
    {
        auto r = mainSC._localized_values.insert(pair);
        if (!r.second)
            r.first->second = pair.second;
    }

    //_localized_values.insert_or_assign(other_values.begin(), other_values.end());
#else
    impl::ScriptParser_DoParse(path, mainSC);
#endif
}

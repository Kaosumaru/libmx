#include "Script.h"
#include "ScriptParser.h"
#include "game/resources/Paths.h"
#include "utils/ListFiles.h"
#include <iostream>
#include <iterator>
#include <memory>
#include <regex>
#include <sstream>
#include <thread>
#include <filesystem>

#ifdef WIPTHREAD
#include "utils/ThreadPool.h"

#endif

void TokenizeByRegex(const std::wstring& text, const std::function<void(const std::wstring&, const std::wstring&, bool)>& t)
{
    wchar_t begin_ch = L'{';
    wchar_t end_ch = L'}';

    std::wstring::size_type current_pos = 0;
    while (true)
    {
        auto begin_pos = text.find(begin_ch, current_pos);
        if (begin_pos == std::wstring::npos)
            break;
        auto end_pos = text.find(end_ch, begin_pos);
        if (end_pos == std::wstring::npos)
            break;

        /*Snap HERE*/
        auto pretext_size = begin_pos - current_pos;

        std::wstring pretext;
        if (pretext_size > 0)
            pretext = text.substr(current_pos, pretext_size);

        std::wstring matched_text = text.substr(begin_pos, end_pos - begin_pos + 1);

        static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
        std::string smatch = convert.to_bytes(matched_text);

        t(pretext, matched_text, true);

        current_pos = end_pos + 1;
        if (current_pos >= text.length())
            break;
    }

    if (current_pos < text.length())
        t(text.substr(current_pos), std::wstring(), false);
}

namespace MX
{
const std::wstring loc(const std::string& label)
{
    return Script::localize(label);
}
};

using namespace MX;

const std::wstring Script::localize(const std::string& label)
{
    return Script::current()._localize(label);
}

bool Script::loadScript(const std::string& file)
{
    return Script::current()._loadScript(file);
}

void Script::Clear()
{
    return Script::current()._localized_values.clear();
}

Signal<void(void)> Script::onParsed;

void Script::ParseDirs(const std::list<std::string>& paths, bool reset)
{
    static std::set<std::string> _allUsedPaths;

    std::list<std::string> previousPaths;
    if (paths.empty())
    {
        for (auto& p : _allUsedPaths)
            previousPaths.push_back(p);
    }
    else
    {
        for (auto& p : paths)
            _allUsedPaths.insert(p);
    }

    //last_write_time(

    static std::map<std::filesystem::path, std::filesystem::file_time_type> _fileStamps;

    if (reset)
    {
        Clear();
        _fileStamps.clear();
    }
    //Clear();

    if (!reset && !_fileStamps.empty())
    {
        for (auto& pair : _fileStamps)
        {
            auto& path = pair.first;
            auto& last_stamp = pair.second;
            auto now_stamp = last_write_time(path);

            if (last_stamp != now_stamp)
            {
                last_stamp = now_stamp;
                Script::loadScript(path.string());
            }
        }

        onParsed();
        return;
    }

    std::vector<std::filesystem::path> file_paths;

    const auto& i_paths = paths.empty() ? previousPaths : paths;
    for (auto& p : i_paths)
    {
        auto pathToP = Paths::get().pathToResource(p);
        using namespace std::filesystem;

        if (!is_directory(pathToP))
            continue;

        for (recursive_directory_iterator i(pathToP), end; i != end; ++i)
        {
            auto& path = i->path();
            if (is_directory(path))
                continue;
 
            if (path.extension() == ".json" || path.extension() == ".msl")
            {
                auto& last_stamp = _fileStamps[path];
                auto now_stamp = last_write_time(path);

                if (last_stamp != now_stamp)
                {
                    last_stamp = now_stamp;
                    file_paths.emplace_back(path);
                }
            }
        }
        
    }

#ifdef MX_MT_IMPL
    static bool first_time = true;

    if (reset || first_time)
    {
        Scriptable::Detail::ValueMember::isOnMainThread = false;
        thread_pool pool(8);
        for (auto& path : file_paths)
            pool.run_task([p = std::move(path)]() {
                Script::loadScript(p);
            });
        pool.join();
        Scriptable::Detail::ValueMember::isOnMainThread = true;

        auto& s = Script::current();
        for (auto& pair : s._localized_values)
            pair.second->member()->finalizeOnMainThread();
    }
    else
    {
        for (auto& path : file_paths)
            Script::loadScript(path);
    }

    first_time = false;
#else
    Scriptable::Detail::ValueMember::isOnMainThread = true;
    for (auto& path : file_paths)
        Script::loadScript(path.string());
#endif

    onParsed();
}

const Scriptable::Value::pointer& Script::propertyOrNull(const std::string& o, const std::string& property)
{
    std::string obj = o;

    static Scriptable::Value::pointer null_pointer;

    while (true)
    {
        auto& return_value = Script::objectOrNull(obj + "." + property);
        if (return_value)
            return return_value;

        auto& parent = Script::objectOrNull(obj + ".Prototype");
        if (!parent || !parent->pointer_to_next())
            return return_value;

        obj = parent->pointer_to_next()->fullPath();
    }
}

const std::wstring Script::_localize(const std::string& label)
{
    auto it = _localized_values.find(label);
    if (it != _localized_values.end())
        return *(it->second);
    static std::wstring temp;
    temp = std::wstring(label.begin(), label.end());
    return temp;
}

const Scriptable::Value& Script::_valueOf(const std::string& label)
{
    return *_object(label);
}

const Scriptable::Value& Script::valueOf(const std::string& label)
{
    return Script::current()._valueOf(label);
}

const Scriptable::Value::pointer& Script::_object(const std::string& label)
{
    static Scriptable::Value::pointer null_pointer;
    //relative path ie <TeleportSkill>.Time
    if (label[0] == '<' && Context<std::string, KeyContext>::isCurrent())
    {
        auto& key = Context<std::string, KeyContext>::current();

        auto closure = label.find('>');
        if (closure == std::string::npos)
            return null_pointer;

        auto match_key = key.rfind(label.c_str() + 1, std::string::npos, closure - 1);

        if (match_key == std::string::npos)
            return null_pointer;

        auto nlabel = key.substr(0, match_key + closure - 1) + label.substr(closure + 1);

        Scriptable::Value::pointer& pointer = _localized_values[nlabel];
        if (pointer == nullptr)
            pointer = std::make_shared<Scriptable::Value>(nlabel);

        return pointer;
    }

    Scriptable::Value::pointer& pointer = _localized_values[label];
    if (pointer == nullptr)
        pointer = std::make_shared<Scriptable::Value>(label);

    return pointer;
}

const Scriptable::Value::pointer& Script::_objectOrNull(const std::string& label)
{
    static Scriptable::Value::pointer null_pointer;
    auto it = _localized_values.find(label);
    if (it == _localized_values.end())
        return null_pointer;
    return it->second;
}

const Scriptable::Value::pointer& Script::object(const std::string& label)
{
    return Script::current()._object(label);
}

const Scriptable::Value::pointer& Script::objectOrNull(const std::string& label)
{
    return Script::current()._objectOrNull(label);
}

bool Script::_valueExists(const std::string& label)
{
    return _localized_values.find(label) != _localized_values.end();
}

bool Script::valueExists(const std::string& label)
{
    return Script::current()._valueExists(label);
}

const Scriptable::Value::pointer& Script::SetObject(const std::string& key, const Scriptable::Value::Map& map)
{
    const auto& object = _object(key);
    object->UpdateObject(map);
    return object;
}

const Scriptable::Value::pointer& Script::SetPair(const std::string& key, bool value)
{
    const auto& object = _object(key);
    object->Update(value);
    return object;
}

const Scriptable::Value::pointer& Script::SetPair(const std::string& key, float value, bool percent)
{
    const auto& object = _object(key);
    object->Update(value, percent);
    return object;
}

const Scriptable::Value::pointer& Script::SetPair(const std::string& key, const std::wstring& value)
{
    const auto& object = _object(key);
    object->Update(value);
    return object;
}

const Scriptable::Value::pointer& Script::SetPair(const std::string& key, const std::vector<Scriptable::Value::pointer>& members)
{
    const auto& object = _object(key);
    object->Update(members);
    return object;
}

const Scriptable::Value::pointer& Script::SetPairFunctor(const std::string& key, const std::function<float()>& number_functor)
{
    const auto& object = _object(key);
    object->Update(number_functor);
    return object;
}

std::wstring formatToValue(const std::string& path, const std::string& label)
{
    std::string objectPath;
    if (label.size() < 2)
        return L"";

    std::string svalue = label.substr(1, label.size() - 2);
    if (svalue.back() == '%')
    {
        svalue.pop_back();
        bool minus = false;
        if (svalue.size() && svalue.front() == L'-')
            svalue.erase(0, 1), minus = true;

        objectPath = path + "." + svalue;
        float value = Script::valueOf(objectPath);
        if (minus)
            value = -value;
        std::wstringstream ss;
        ss << value * 100.0 << L'%';
        return ss.str();
    }

    objectPath = path + "." + svalue;
    return Script::object(objectPath)->text();
}

const std::wstring Script::parseString(const std::string& path, const std::wstring& text)
{
    std::wstringstream ss;
    TokenizeByRegex(text, [&](const std::wstring& t, const std::wstring& match, bool matched) {
        ss << t;
        if (matched)
        {
            static std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
            std::string smatch = convert.to_bytes(match);
            ss << formatToValue(path, smatch);
        }
    });
    return ss.str();
}

bool Script::_loadScript(const std::string& file)
{
    ScriptParser::ParseFileToScript(file, *this);

    return true;
}
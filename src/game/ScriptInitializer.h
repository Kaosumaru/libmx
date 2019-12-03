#pragma once

namespace MX
{

class ScriptInitializer
{
public:
    static void Init();
    static void AfterScriptParse();
    static void ReloadScripts(bool reset = false);
    static void RegisterShortcuts();
};

}

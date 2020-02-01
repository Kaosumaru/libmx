#include "Save.h"
#include "utils/Log.h"
#include <mutex>
#include <set>

using namespace MX;

std::filesystem::path MX::getConfigHomePath()
{
    char* appdata = getenv("APPDATA");
    return appdata;
}

std::filesystem::path MX::getSavePath()
{
    auto path = getConfigHomePath() / MX_APPLICATION_NAME;
#ifdef _DEBUG
    path += "_DEBUG";
#endif
    return path;
}

#ifdef _DEBUG
namespace
{
std::set<std::filesystem::path> saves_paths;
std::recursive_mutex saves_paths_mutex;
}
#endif

BaseSave::BaseSave(const std::filesystem::path& name)
{
    _path = getSavePath() / name;

#ifdef _DEBUG
    std::lock_guard lock(saves_paths_mutex);
    auto [it, b] = saves_paths.insert(_path);
    if (!b)
    {
        spdlog::error("Created save with duplicated name: {}", name.string());
    }
#endif
}

BaseSave::~BaseSave()
{
#ifdef _DEBUG
    std::lock_guard lock(saves_paths_mutex);
    saves_paths.erase(_path);
#endif
}

bool BaseSave::Exists()
{
    return std::filesystem::exists(_path);
}

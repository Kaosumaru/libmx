#include "ListFiles.h"
#include "application/Application.h"
#include "utils/Time.h"
#include "utils/Random.h"

namespace MX
{

FileObserver::FileObserver()
{
}

FileObserver::FileObserver(const std::filesystem::path& path)
    : FileObserver()
{
    RegisterPath(path);
}

FileObserver::pointer FileObserver::Create()
{
    auto observer = std::make_shared<FileObserver>();
    observer->WaitThenObserve();
    return observer;
}
FileObserver::pointer FileObserver::Create(const std::filesystem::path& path)
{
    auto observer = std::make_shared<FileObserver>(path);
    observer->WaitThenObserve();
    return observer;
}

void FileObserver::RegisterPath(const std::filesystem::path& path)
{
    _paths.push_back({ path, std::filesystem::last_write_time(path) });
}

void FileObserver::WaitThenObserve()
{
    float time = Random::randomRange(std::pair{0.4f, 0.5f});
    auto& queue = MX::App::current().queue();
    queue.planWeakFunctor(
        time, [&]() {
            Observe();
            WaitThenObserve();
        },
        shared_from_this());
}

void FileObserver::Observe()
{
    bool changed = false;
    for (auto& [path, stamp] : _paths)
    {
        auto new_stamp = std::filesystem::last_write_time(path);
        if (new_stamp == stamp)
            continue;
        stamp = new_stamp;
        changed = true;
        onFileChanged(path);
    }
    if (changed)
        onFilesChanged();
}

std::string FileExtension(const std::string& path)
{
    auto idx = path.rfind('.');

    if (idx != std::string::npos)
        return path.substr(idx + 1);
    return "";
}

}

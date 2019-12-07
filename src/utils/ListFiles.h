#pragma once

#include "Signal.h"
#include <ctime>
#include <filesystem>
#include <functional>
#include <vector>

namespace MX
{

// TODO move observing on threadpool
class FileObserver : public shared_ptr_init<FileObserver>
{
public:
    FileObserver();
    FileObserver(const std::filesystem::path& path);

    static pointer Create();
    static pointer Create(const std::filesystem::path& path);

    void RegisterPath(const std::filesystem::path& path);

    Signal<void(const std::filesystem::path& path)> onFileChanged;
    Signal<void()> onFilesChanged;

protected:
    void WaitThenObserve();
    void Observe();

    struct PathData
    {
        std::filesystem::path path;
        std::filesystem::file_time_type timestamp;
    };

    std::vector<PathData> _paths;
};

std::string FileExtension(const std::string& path);
}

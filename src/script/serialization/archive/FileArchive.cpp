#include "FileArchive.h"
#include <fstream>

using namespace MX;
using namespace Serialization;

//TODO async
void FileArchive::Load(const LoadCallback& cb)
{
    auto worker = [cb, path = path()]() {
        std::fstream file { path, std::ios_base::in | std::ios_base::binary };
        if (file.fail())
        {
            cb("", Result::PathNotFound);
            return;
        }
        std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (file.fail())
        {
            cb("", Result::PathNotFound);
            return;
        }
        cb(std::move(data), Result::OK);
    };
    worker();
}

void FileArchive::Save(std::string&& data, const SaveCallback& cb)
{
    auto worker = [data = std::move(data), cb, path = path()]() {
        std::fstream file { path, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary };
        if (file.fail())
        {
            cb(Result::ErrorFatal);
            return;
        }
        file << data;
        if (file.fail())
        {
            cb(Result::ErrorFatal);
            return;
        }
        cb(Result::OK);
    };
    worker();
}
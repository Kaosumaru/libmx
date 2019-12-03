#pragma once
#include "script/serialization/archive/Archive.h"

namespace MX
{
namespace Serialization
{
    class EmscriptenArchive : public Archive
    {
    public:
        using Archive::Archive;
        void Load(const LoadCallback& cb) override;
        void Save(std::string&& data, const SaveCallback& cb) override;
    };
}
}

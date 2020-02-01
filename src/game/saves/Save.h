#pragma once
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <filesystem>
#include <memory>

namespace MX
{
std::filesystem::path getConfigHomePath();
std::filesystem::path getSavePath();

class BaseSave
{
protected:
    BaseSave(const std::filesystem::path& name);
    ~BaseSave();

public:
    bool Exists();

protected:
    std::filesystem::path _path;
};

template <typename T>
class Save : public BaseSave
{
public:
#if 0
    using InArchive = cereal::BinaryInputArchive;
    using OutputArchive = cereal::BinaryOutputArchive;
#else
    using InArchive = cereal::JSONInputArchive;
    using OutputArchive = cereal::JSONOutputArchive;

#endif
    using DataPointer = std::shared_ptr<T>;

    Save(const std::filesystem::path& name)
        : BaseSave(name)
    {
    }

    bool SaveSync()
    {
        if (!_data)
            return false;
        std::filesystem::create_directories(getSavePath());
        std::ofstream os(_path, std::ios::binary);
        OutputArchive archive(os);

        archive(cereal::make_nvp("data", *_data));
    }

    bool Load()
    {
        std::ifstream is(_path, std::ios::binary);
        if (!is)
            return false;
        InArchive archive(is);

        if (!_data)
            _data.reset(new T);
        archive(cereal::make_nvp("data", *_data));
        return true;
    }

    void Create(const T& data)
    {
        _data.reset(new T(data));
    }

    void Create()
    {
        _data.reset(new T);
    }

    const DataPointer& data() { return _data; }

protected:
    DataPointer _data;
};

}
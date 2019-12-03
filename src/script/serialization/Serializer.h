#pragma once
#include "utils/Singleton.h"
#include <cstdint>
#include <string>

namespace MX
{
namespace Serialization
{
    class Node;

    class Serializer : public ScopeSingleton<Serializer>
    {
    public:
        Serializer() { }
        virtual ~Serializer() { }

        virtual bool loading() { return true; }
        bool saving() { return !loading(); }

        virtual void Sync(Node& n, int& v) {};
        virtual void Sync(Node& n, int64_t& v) {};
        virtual void Sync(Node& n, double& v) {};
        virtual void Sync(Node& n, std::string& v) {};
        virtual void Sync(Node& n, std::wstring& v) {};
    };
}
}

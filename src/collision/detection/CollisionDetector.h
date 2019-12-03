#pragma once
#include "utils/Singleton.h"
#include <functional>
#include <map>
namespace MX
{
namespace Collision
{

    class Shape;

    class Detection : public Singleton<Detection>
    {
    public:
        using DetectionFunction = bool (*)(const Shape& shape1, const Shape& shape2);
        Detection();

        template <typename Class1, typename Class2, DetectionFunction function>
        void AddCollision()
        {
            using namespace std;
            using namespace std::placeholders;
            _functionForIDs[std::make_pair(ClassID<Class1>::id(), ClassID<Class2>::id())] = function;
            if (ClassID<Class2>::id() != ClassID<Class1>::id())
                _functionForIDs[std::make_pair(ClassID<Class2>::id(), ClassID<Class1>::id())] = [](const Shape& shape1, const Shape& shape2) { return function(shape2, shape1); };
        }

        bool checkCollision(const Shape& shape1, const Shape& shape2);

    protected:
        void Init();
        std::map<std::pair<ClassID<>::type, ClassID<>::type>, DetectionFunction> _functionForIDs;
    };

}
};

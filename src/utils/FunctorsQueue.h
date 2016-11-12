#pragma once
#include<memory>
#include<random>
#include<map>
#include<functional>
#include <thread>
#include <mutex>


#include "scene/Actor.h"
#include "utils/Singleton.h"

namespace MX
{

class FunctorsQueue : public Actor, public ScopeSingleton<FunctorsQueue>
{
public:
	typedef std::function< void(void) > Functor;

	FunctorsQueue();


	void planFunctor(double inSeconds, const Functor& functor);
	void queueFunctor(const Functor& functor);

	template<typename T>
	void planWeakFunctor(double inSeconds, const Functor& functor, const std::shared_ptr<T> &object)
	{
		std::weak_ptr<T> weak(object);
		auto wrapper_functor = [=]()
		{
			auto lock = weak.lock();
			if (lock)
				functor();
		};
		planFunctor(inSeconds, wrapper_functor);
	}


	void Run();

	bool empty();
protected:
	

	std::multimap<double, Functor> _plannedFunctors;
	std::list<Functor> _queuedFunctors;
	std::mutex _mutex;
};


};


#include "FunctorsQueue.h"
#include "utils/Time.h"

using namespace MX;
using namespace std;


FunctorsQueue::FunctorsQueue()
{
}

void FunctorsQueue::Run()
{
	{
		lock_guard<mutex> lock(_mutex); 
		std::list<Functor> movedList;
		std::swap(movedList, _queuedFunctors);


		_mutex.unlock();
		for (auto &f : movedList)
			f();
		_mutex.lock();

		auto seconds = Time::Timer::current().total_seconds() ;
		auto pit = _plannedFunctors.begin();
		auto end = _plannedFunctors.end();
		for (; pit != end; pit ++)
		{
			if (pit->first > seconds)
				break;
			_mutex.unlock();
			pit->second();
			_mutex.lock();
		}

		if (pit != _plannedFunctors.begin())
			_plannedFunctors.erase(_plannedFunctors.begin(), pit);
	}

	Actor::Run();
}

bool FunctorsQueue::empty()
{
	lock_guard<mutex> lock(_mutex);
	return _queuedFunctors.empty() && _plannedFunctors.empty();
}

void FunctorsQueue::planFunctor(double inSeconds, const Functor& functor)
{
	if (inSeconds == 0.0)
	{
		queueFunctor(functor);
		return;
	}

	lock_guard<mutex> lock(_mutex);
	//auto time = Time::Timer::current().elapsed_seconds() + inSeconds;
	auto time = Time::Timer::current().total_seconds() + inSeconds;

	_plannedFunctors.insert(make_pair(time, functor));
}

void FunctorsQueue::queueFunctor(const Functor& functor)
{
	lock_guard<mutex> lock(_mutex);
	_queuedFunctors.push_back(functor);
}


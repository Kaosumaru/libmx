#pragma once
#include<list>
#include<memory>
#include<functional>
#include<cassert>
#include"Utils.h"

namespace MX
{
	template <typename F>
	struct ScopeExit {
		ScopeExit(F &&f) : f(std::forward<F>(f)) {}
		~ScopeExit() { f(); }
		F f;
	};

	template <typename F>
	ScopeExit<F> AtScopeExit(F&&f)
	{
		return ScopeExit<F>(std::forward<F>(f));
	}

	template<typename T>
	class Signal
	{

	};

	class SignalTrackable
	{
	private:
		template<typename T>
		friend class Signal;

		const auto &trackable_data()
		{
			if (!_trackable_data)
				_trackable_data = std::make_shared<int>();
			return _trackable_data;
		}
		std::shared_ptr<int> _trackable_data;
	};

	struct Connection
	{
		using pointer = std::shared_ptr<Connection>;
		void disconnect() { _connected = false; }
		bool connected() const { return _connected; }
	protected:
		bool _connected = true;
	};



	template<typename R, typename ...Args>
	class Signal<R(Args...)>
	{
	public:
		using Function = std::function<R(Args...)>;
		using FunctionExtended = std::function<R(Connection&, Args...)>;
	protected:


		struct FunctorEntryBase : public Connection
		{
			//return false if you want to be unlinked
			virtual bool Do(Args... arg)
			{
				return true;
			}
		};

		struct FunctorEntry : public FunctorEntryBase
		{
			FunctorEntry(const typename Signal::Function& func) : _func(func)
			{
			}

			virtual bool Do(Args... arg) override
			{
				if (!Connection::_connected)
					return false;
				_func(std::forward<Args>(arg)...);
				return true;
			}

			typename Signal::Function _func;
		};

		template<typename T>
		struct FunctorEntryWeak : public FunctorEntry
		{
			FunctorEntryWeak(const typename Signal::Function& func, const std::weak_ptr<T> &ptr) : FunctorEntry(func), _ptr(ptr) {}
			bool Do(Args... arg) override
			{
				if (!Connection::_connected)
					return false;
				if (auto ptr = _ptr.lock())
				{
					this->_func(std::forward<Args>(arg)...);
					return true;
				}
				return false;
			}
			std::weak_ptr<T> _ptr;
		};

		struct FunctorEntryExtended : public FunctorEntryBase
		{
			FunctorEntryExtended(const typename Signal::FunctionExtended& func) : _func(func)
			{
			}

			bool Do(Args... arg) override
			{
				if (!Connection::_connected)
					return false;
				_func(*this, std::forward<Args>(arg)...);
				return true;
			}

			typename Signal::FunctionExtended _func;
		};

		template<typename T>
		struct FunctorEntryWeakExtended : public FunctorEntryExtended
		{
			FunctorEntryWeakExtended(const typename Signal::FunctionExtended& func, const std::weak_ptr<T> &ptr) : FunctorEntry(func), _ptr(ptr) {}
			bool Do(Args... arg) override
			{
				if (!Connection::_connected)
					return false;
				if (auto ptr = _ptr.lock())
				{
					this->_func(*this, std::forward<Args>(arg)...);
					return true;
				}
				return false;
			}
			std::weak_ptr<T> _ptr;
		};

	public:
		Connection::pointer static_connect_front(const Function& func)
		{
			_functors.emplace_front(std::make_shared<FunctorEntry>(func));
			return _functors.front();
		}

		Connection::pointer static_connect(const Function& func)
		{
			_functors.emplace_back(std::make_shared<FunctorEntry>(func));
			return _functors.back();
		}

		Connection::pointer static_connect_extended(const FunctionExtended& func)
		{
			_functors.emplace_back(std::make_shared<FunctorEntryExtended>(func));
			return _functors.back();
		}

		Connection::pointer connect_front(const Function& func, SignalTrackable* obj)
		{
			return connect_front(func, obj->trackable_data());
		}

		Connection::pointer connect(const Function& func, SignalTrackable* obj)
		{
			return connect(func, obj->trackable_data());
		}

		template<typename T>
		Connection::pointer connect_front(const Function& func, const std::shared_ptr<T> &ptr)
		{
			_functors.emplace_front(std::make_shared<FunctorEntryWeak<T>>(func, ptr));
			return _functors.front();
		}

		template<typename T>
		Connection::pointer connect(const Function& func, const std::shared_ptr<T> &ptr)
		{
			_functors.emplace_back(std::make_shared<FunctorEntryWeak<T>>(func, ptr));
			return _functors.back();
		}

		template<typename T>
		Connection::pointer connect_extended(const FunctionExtended& func, const std::shared_ptr<T> &ptr)
		{
			_functors.emplace_back(std::make_shared<FunctorEntryExtended>(func, ptr));
			return _functors.back();
		}

		void operator() (Args... arg)
		{
			if (_functors.empty())
				return;
			_iterations++;
			const auto &g = AtScopeExit([&]() { _iterations--; }); ///slots can throw, so decrement _iterations at scope exit

			auto it = _functors.begin();
			while (it != _functors.end())
			{
				if (!(*it)->Do(std::forward<Args>(arg)...))
				{
					(*it)->disconnect();
					if (_iterations == 1)
					{
						it = _functors.erase(it);
						continue;
					}
				}

				it++;
			}

		}


		void disconnect_all_slots()
		{
			//TODO
			assert(_iterations == 0);
			_functors.clear();
		}
	protected:
		int _iterations = 0;
		std::list<std::shared_ptr<FunctorEntryBase>> _functors;
	};



	template <typename... Args>
	class SimpleSignal
	{
	public:
		typedef std::function<void(Args...)> Function;
	protected:
		struct FunctorEntry
		{
			FunctorEntry(const typename SimpleSignal::Function& func) : _func(func) {}
			virtual bool Do(Args&&... arg)
			{
				_func(std::forward<Args>(arg)...);
				return true;
			}

			typename SimpleSignal::Function _func;
		};

		template<typename T>
		struct FunctorEntryWeak : public FunctorEntry
		{
			FunctorEntryWeak(const typename SimpleSignal::Function& func, const std::weak_ptr<T> &ptr) : FunctorEntry(func), _ptr(ptr) {}
			bool Do(Args&&... arg)
			{
				if (auto ptr = _ptr.lock())
				{
					this->_func(std::forward<Args>(arg)...);
					return true;
				}
				return false;
			}
			std::weak_ptr<T> _ptr;
		};
	public:


		void connect(const Function& func)
		{
			_functors.emplace_back(std::make_shared<FunctorEntry>(func));
		}

		template<typename T>
		void connect(const Function& func, const std::shared_ptr<T> &ptr)
		{
			_functors.emplace_back(std::make_shared<FunctorEntryWeak<T>>(func, ptr));
		}

		void operator() (Args&&... arg)
		{
			if (_functors.empty())
				return;
			auto it = _functors.begin();
			while (it != _functors.end())
			{
				if (!(*it)->Do(std::forward<Args>(arg)...))
					it = _functors.erase(it);
				else
					it++;
			}
		}

		void disconnect_all_slots()
		{
			_functors.clear();
		}

	protected:
		std::list<std::shared_ptr<FunctorEntry>> _functors;
	};
};
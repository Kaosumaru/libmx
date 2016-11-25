#ifndef MXEVENT
#define MXEVENT

#include <memory>
#include "Script/MXScriptObject.h"
#include "Script/Scriptable/MXScriptableValue.h"
#include "Script/MXPropertyLoaders.h"
#include "Utils/MXBoostFast.h"
#include "MXCommand.h"

namespace bs2 = boost::signals2;

namespace MX
{
	struct EventInfo
	{
		EventInfo() {}
		int returnValue = 0;
	};

	class Event : public Command, public bs2::trackable
	{
	public:
		Event(){};
		Event(const std::string& objectName) : Command(objectName) {}

		bool operator () ()
		{
			Do();
			return true;
		}
        
		virtual void Do(){}


		void Cancel() { _canceled = true; }
		bool canceled() { return _canceled; }

		Command::pointer clone() override { return shared_from_this(); }
	protected:
		bool _canceled = false;
	};

	typedef std::shared_ptr<Event> EventPtr;


	class EventHolder : public Event
	{
	public:
		EventHolder(){};
        EventHolder(const std::string& objectName) : Event(objectName) 
        {
            load_property_children(_events, "Events");
        };
		virtual ~EventHolder(){}

		void SetEvents(const std::vector<EventPtr>& events)
		{
			_events = events;
		}

		void AddEvents(const std::vector<EventPtr>& events)
		{
			for (auto &e : events)
				AddEvent(e);
		}

		void AddEvents(const EventHolder& holder)
		{
			AddEvents(holder._events);
		}

		void Clear()
		{
			_events.clear();
		}

		void AddEvent(const EventPtr& ptr)
		{
			_events.push_back(ptr);
		}

		bool empty() const
		{
			return _events.empty();
		}


		void Do()
		{
			auto it = _events.begin();
			while (it != _events.end())
			{
				if ((*it)->canceled())
				{
					it = _events.erase(it);
					continue;
				}
				(*it)->Do();
				++ it;
			}
		}

	protected:
		std::vector<EventPtr> _events;
	};

	template<>
	struct PropertyLoader<EventHolder>
	{
		using type = PropertyLoader_Standard;
		static bool load(EventHolder& out, const Scriptable::Value::pointer& obj)
		{
			if (obj->size() > 0)
			{
				std::vector<EventPtr> events;
				PropertyLoader< std::vector<EventPtr> >::load(events, obj);
				out.SetEvents(events);
				return true;
			}
			else
			{
				out.Clear();
				auto event = obj->to_object<Event>();
				if (event)
					out.AddEvent(event);
				return true;
			}

			return false;
		}
	};


	class EventInit
	{
	public:
		static void Init();
	};


}


#endif

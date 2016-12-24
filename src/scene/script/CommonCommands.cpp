#include "CommonCommands.h"
#include <algorithm>
#include "utils/Time.h"
#include "script/ScriptClassParser.h"

//TODO 
#undef _t

namespace MX
{
	using namespace std;

	/*
	** Waits given time, then dies
	*/

	WaitCommand::WaitCommand(float seconds) : _seconds(seconds) {  }
	WaitCommand::WaitCommand(LScriptObject& script) { script.load_property(_seconds, "Time"); }
	WaitCommand::WaitCommand(const WaitCommand& other) : _seconds(other._seconds) {  }

	void WaitCommand::Restart()
	{
		_stopWatch.Start(_seconds);
	}

	bool WaitCommand::operator () ()
	{
		if (!_stopWatch.isSet())
			Restart();
		auto ret = !_stopWatch.Tick();
		OnWait();
		return ret;
	}

	float WaitCommand::percent()
	{
		return _stopWatch.percent();
	}

	class QueueCommand : public CompositeCommand
	{
	public:
		QueueCommand(const QueueCommand& other) : CompositeCommand(other){}
		QueueCommand(LScriptObject& script) : CompositeCommand(script){}
		QueueCommand(list<Command::pointer> &&c) : CompositeCommand(std::move(c)){}

		bool operator () ()
		{
			while (!_commands.empty())
			{
				if ((*_commands.front())())
					return true;
				_commands.pop_front();
			}
			return false;
		}

		Command::pointer clone()
		{
			return std::make_shared<QueueCommand>(*this);
		}
	};


	class LoopCommand : public CompositeCommand
	{
	public:
		LoopCommand(const LoopCommand& other) : _iterations(other._iterations), CompositeCommand(other){}
		LoopCommand(LScriptObject& script) : CompositeCommand(script){ script.load_property(_iterations, "Iterations"); }
		LoopCommand(list<Command::pointer> &&c, int i) : _iterations(i+1), CompositeCommand(std::move(c)){}
		LoopCommand(list<Command::pointer> &&c) : CompositeCommand(std::move(c)){}

		virtual void Restart()
		{
			if (_iterations > 0)
				_iterations--;
			for_each(_commands.begin(), _commands.end(), [](const Command::pointer &ptr){ptr->Restart(); });
			current = _commands.begin();
		}


		bool operator () ()
		{
			if (_iterations == 0)
				return false;
			if (current == _commands.end())
				Restart();

			if (current == _commands.end())
				return true;

			while (!(**current)())
			{
				current++;
				if (current == _commands.end())
					return true;
			}

			return true;
		}

		Command::pointer clone()
		{
			return std::make_shared<LoopCommand>(*this);
		}

	protected:
		list<Command::pointer>::iterator current = _commands.end();
		int _iterations = -1;
	};

	class SimultaneusCommand : public CompositeCommand
	{
	public:
		SimultaneusCommand(const SimultaneusCommand& other) : CompositeCommand(other){}
		SimultaneusCommand(LScriptObject& script) : CompositeCommand(script){}
		SimultaneusCommand(list<Command::pointer> &&c) : CompositeCommand(std::move(c)) {}


		bool operator () ()
		{
			auto it = _commands.begin();
			while (it != _commands.end())
			{
				if (!(*(*it))())
					it = _commands.erase(it);
				else
					++it;
			}
			return !_commands.empty();
		}

		Command::pointer clone()
		{
			return std::make_shared<SimultaneusCommand>(*this);
		}
	};

	Command::pointer wait(float seconds)
	{
		return make_shared<WaitCommand>(seconds);
	}



	void CommonCommandsInit::Init()
	{
		ScriptClassParser::AddCreator(L"Command.Wait", new OutsideScriptClassCreatorContructor<WaitCommand>());
		ScriptClassParser::AddCreator(L"Command.Queue", new OutsideScriptClassCreatorContructor<QueueCommand>());
		ScriptClassParser::AddCreator(L"Command.Set", new OutsideScriptClassCreatorContructor<SimultaneusCommand>());
		ScriptClassParser::AddCreator(L"Command.Loop", new OutsideScriptClassCreatorContructor<LoopCommand>());
	}



	
	class LambdaCommand : public Command
	{
	public:
		typedef std::function<bool(void)> Function;
		LambdaCommand(const Function &t) : _t(t) {}
		bool operator()() { return _t(); }

		Command::pointer clone() override { return std::make_shared<LambdaCommand>(_t); }
	protected:
		Function _t;
	};

	Command::pointer command_from_functor(const std::function<bool(void)> &f)
	{
		return std::make_shared<LambdaCommand>(f);
	}


	std::shared_ptr<CompositeCommand> l(std::list<Command::pointer> &&l)
	{
		return std::make_shared<LoopCommand>(std::move(l));
	}

	std::shared_ptr<CompositeCommand> l(int i, std::list<Command::pointer> &&l)
	{
		return std::make_shared<LoopCommand>(std::move(l), i);
	}

	std::shared_ptr<CompositeCommand> s(std::list<Command::pointer> &&l)
	{
		return std::make_shared<SimultaneusCommand>(std::move(l));
	}
	std::shared_ptr<CompositeCommand> q(std::list<Command::pointer> &&l)
	{
		return std::make_shared<QueueCommand>(std::move(l));
	}

}
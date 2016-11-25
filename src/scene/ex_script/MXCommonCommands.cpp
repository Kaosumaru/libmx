#include "MXCommonCommands.h"

#include "Utils/MXInclude.h"
#include <algorithm>

#include "Utils/MXTime.h"
#include "Script/MXScriptClassParser.h"

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


	class CompositeCommand : public Command
	{
	public:
		CompositeCommand(const CompositeCommand& other)
		{
			_commands = other._commands;
			for (auto &command : _commands)
				command = command->clone();
		}

		CompositeCommand(LScriptObject& script)
		{
			script.load_property_children(_commands, "Commands");
		}

		CompositeCommand(list<Command::pointer> &&c) : _commands(c)
		{
		}
	protected:
		list<Command::pointer> _commands;
	};

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
			return MX::make_shared<QueueCommand>(*this);
		}
	};


	class LoopCommand : public CompositeCommand
	{
	public:
		LoopCommand(const LoopCommand& other) : CompositeCommand(other){}
		LoopCommand(LScriptObject& script) : CompositeCommand(script){}
		LoopCommand(list<Command::pointer> &&c) : CompositeCommand(std::move(c)){}

		void Restart()
		{
			for_each(_commands.begin(), _commands.end(), [](const Command::pointer &ptr){ptr->Restart(); });
			current = _commands.begin();
		}


		bool operator () ()
		{
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
			return MX::make_shared<LoopCommand>(*this);
		}


	protected:
		list<Command::pointer>::iterator current = _commands.end();
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
			return MX::make_shared<SimultaneusCommand>(*this);
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

		Command::pointer clone() { return MX::make_shared<LambdaCommand>(_t); }
	protected:
		Function _t;
	};

	Command::pointer command_from_functor(const std::function<bool(void)> &f)
	{
		return std::make_shared<LambdaCommand>(f);
	}


	Command::pointer l(std::list<Command::pointer> &&l)
	{
		return std::make_shared<LoopCommand>(std::move(l));
	}
	Command::pointer s(std::list<Command::pointer> &&l)
	{
		return std::make_shared<SimultaneusCommand>(std::move(l));
	}
	Command::pointer q(std::list<Command::pointer> &&l)
	{
		return std::make_shared<QueueCommand>(std::move(l));
	}

}
#pragma once
#include "Command.h"
#include "utils/Utils.h"
#include <memory>
#include "utils/Time.h"
#include "utils/Graphic/Easings.h"


namespace MX
{

	/*
	** Waits given time, then dies
	*/
	class WaitCommand : public Command
	{
	public:
		WaitCommand(float seconds);
		WaitCommand(LScriptObject& script);
		WaitCommand(const WaitCommand& other);
		void Restart() override;
		bool operator () () override;

		float percent();
		Command::pointer clone() override { return std::make_shared<WaitCommand>(*this); }
	protected:
		virtual void OnWait() {}

		float _seconds;
		Time::ManualStopWatch _stopWatch;
	};


	class EasingWaitCommand : public WaitCommand
	{
	public:
		EasingWaitCommand(float time) : WaitCommand(time)
		{

		}

		EasingWaitCommand(const EasingWaitCommand &other) : _easingFunc(other._easingFunc), WaitCommand(other)
		{

		}

		float percent()
		{
			return _easingFunc(0.0f, 1.0f, WaitCommand::percent());
		}
	protected:
		Easing::Function _easingFunc = Easing::linear<float>;
	};

	std::shared_ptr<Command> wait(float seconds);

	class CommonCommandsInit
	{
	public:
		static void Init();
	};


	Command::pointer command_from_functor(const std::function<bool(void)> &f);

	//std::shared_ptr<Command> l(p0, p1, ..., pn);
Command::pointer l(std::list<Command::pointer> &&l);
Command::pointer l(int i, std::list<Command::pointer> &&l);
Command::pointer s(std::list<Command::pointer> &&l);
Command::pointer q(std::list<Command::pointer> &&l);



	

}

#pragma once
//small hack, fixes clashes with our context

#include "Version.h"
#include "Utils/Time.h"
#include "utils/Signal.h"
#include "utils/Singleton.h"
#include "Utils/FunctorsQueue.h"


namespace MX
{
	class Window;

	namespace Graphic
	{
		class Image;
	}

	struct RenderQueue {};

	class App : public ScopeSingleton<App>
	{
	private:
		bool	running;

	public:
		App();
		virtual ~App();

		static const MX::Version& version() { return _version; }
		static const std::string& programName() { return _programName; }

		bool OpenWindow(unsigned width, unsigned height, bool fullscreen = true);
		inline bool IsRunning() { return running; }
		void Quit();

		bool Run();

		void SetCursor(const std::shared_ptr<MX::Graphic::Image>& cursor);

		auto& queue() { return _applicationQueue; }
	protected:
		static void SetProgramName(const std::string& name) { _programName = name; }

		virtual void OnPrepare();
		virtual bool OnInit();
		virtual void OnLoop();
		virtual void OnRender();
		virtual void OnCleanup();

		static std::string _programName;
		static MX::Version _version;

		std::shared_ptr<Window> _window;
		Time::SimpleTimer _timer;
		FunctorsQueue _renderQueue;
		FunctorsQueue _applicationQueue;

		std::shared_ptr<MX::Graphic::Image> _cursor;
	};
}

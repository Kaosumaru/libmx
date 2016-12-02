#pragma once
//small hack, fixes clashes with our context

#include "Version.h"
#include "utils/Time.h"
#include "utils/Signal.h"
#include "utils/Singleton.h"
#include "utils/FunctorsQueue.h"


namespace MX
{
	class Window;
	class Script;

	namespace Graphic
	{
		class Image;
		class TextureRenderer;
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

		bool OpenMainWindow(unsigned width, unsigned height, bool fullscreen = true);
		inline bool IsRunning() { return running; }
		void Quit();

		bool Run();

		void SetCursor(const std::shared_ptr<MX::Graphic::Image>& cursor);

		auto& queue() { return _applicationQueue; }
	protected:
		void Loop();

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
		std::unique_ptr<Script> _script;

		std::shared_ptr<MX::Graphic::Image> _cursor;
		std::shared_ptr<MX::Graphic::TextureRenderer> _defaultRenderer;
	};
}

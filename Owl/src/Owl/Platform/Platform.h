#pragma once

namespace Owl
{
	struct WindowProps
	{
		const char* ApplicationName = "Owl Application";
		int Width = 800;
		int Height = 600;
		int X = 0;
		int Y = 0;
	};

	class Platform
	{
		static void Initialize(const WindowProps& pWindowProps);
		static void Shutdown();

		virtual bool PumpMessages() = 0;

		virtual void Sleep(unsigned long pMilliseconds) = 0;

		static Platform* Get() { return s_Instance; }

		static Platform* s_Instance;

		friend class Application;
	};
}

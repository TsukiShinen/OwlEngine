#pragma once

#include <cstdint>

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
	public:
		virtual void PumpMessages() = 0;

		virtual void Sleep(unsigned long pMilliseconds) = 0;

		static Platform* Create(const WindowProps& pWindowProps);

	private:
		friend class Memory;
	};
}

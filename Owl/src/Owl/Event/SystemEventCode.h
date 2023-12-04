#pragma once


namespace Owl
{
	enum SystemEventCode
	{
		EventCodeApplicationQuit = 0x01,
		EventCodeKeyPressed = 0x02,
		EventCodeKeyReleased = 0x03,
		EventCodeMouseButtonPressed = 0x04,
		EventCodeMouseButtonReleased = 0x05,
		EventCodeMouseMoved = 0x06,
		EventCodeMouseWheel = 0x07,
		EventCodeResized = 0x08,

		MaxEventCode = 0xFF
	};
}

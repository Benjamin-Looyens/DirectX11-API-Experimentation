#pragma once

// Based on https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
namespace stdConsoleColors {
	constexpr int BLACK		= 30;
	constexpr int RED		= 31;
	constexpr int GREEN		= 32;
	constexpr int YELLOW	= 33;
	constexpr int BLUE		= 34;
	constexpr int MAGENTA	= 35;
	constexpr int CYAN		= 36;
	constexpr int WHITE		= 37;

	constexpr int BRIGHT_BLACK		= 90;
	constexpr int BRIGHT_RED		= 91;
	constexpr int BRIGHT_GREEN		= 92;
	constexpr int BRIGHT_YELLOW		= 93;
	constexpr int BRIGHT_BLUE		= 94;
	constexpr int BRIGHT_MAGENTA	= 95;
	constexpr int BRIGHT_CYAN		= 96;
	constexpr int BRIGHT_WHITE		= 97;
}

namespace dualRasterColors {
	constexpr int FPS_COLOR			= stdConsoleColors::WHITE;
	constexpr int CONSOLE_COLOR		= stdConsoleColors::BRIGHT_BLACK;
	constexpr int SHARED_COLOR		= stdConsoleColors::YELLOW;
	constexpr int HARDWARE_COLOR	= stdConsoleColors::GREEN;
	constexpr int SOFTWARE_COLOR	= stdConsoleColors::MAGENTA;
}
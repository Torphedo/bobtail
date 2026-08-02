#pragma once
#include "platform.h"

/// @brief Emulates the Windows "pause" command
///
/// This function blocks until a single key is pressed in the console.
static void console_pause();

#if defined(PLATFORM_LINUX)
#include <stdio.h>
#include <termios.h>

static void console_pause() {
	// Get current stdin terminal settings
    struct termios oldt;
    tcgetattr( STDIN_FILENO, &oldt);

	// Disable "canonical mode", letting us read input without waiting for a newline
    // Also disable echoing (the typed character won't appear in the terminal)
    struct termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO );

	// Apply settings (TCSANOW makes it apply immediately)
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    getchar();

    // Restore old settings
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

#elif defined(PLATFORM_WINDOWS)
#include <conio.h>

static void console_pause() {
    char c = getch();
}

#endif

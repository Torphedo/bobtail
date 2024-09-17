#ifndef LOGGING_H
#define LOGGING_H
/// @file logging.h
/// A logging wrapper allowing color coding, automatic insertion of the current
/// function name, and the ability to silence all output.
#include <stdatomic.h>

/// Color-codes the function name red
static const char error[] = "31";
/// Color-codes the function name yellow
static const char warning[] = "33";
/// Color-codes the function name green
static const char info[] = "32";
/// Color-codes the function name blue
static const char debug[] = "34";

/// The function used by the LOG_MSG() macro
/// @param type Color code to show the type of message
/// @param function The name of the function printing the message. You should
/// usually pass in "__func__" which will automatically be the current function
/// name as a string.
/// @param format_str A printf() style format string
/// @param ... Extra arguments to use with the format string, printf() style.
int logging_print(const char* type, const char* function, const char* format_str, ...);

/// A simple logging utility for color-coded output that automatically logs the
/// function name. The outer interface is a macro to avoid passing "__func__"
/// every time. Usage is identical to printf() but with a message type first.<br>
/// For main(), LOG_MSG(info, "num = %d\n", 5); would print:<br>
/// "\033[32mmain()\033[0m: num = 5\n"<br>
/// In the console, this appears as "main(): num = 5" with "main" colored green.

/// @param type These are all constants you can pass to color-code the function
///             name:<br>
///        @ref error -> red<br>
///        @ref warning -> yellow<br>
///        @ref info -> green<br>
///        @ref debug -> blue
/// @param ... A format string and extra arguments, just like printf().
#define LOG_MSG(type, ...) logging_print(type, __func__, __VA_ARGS__)

/// Enables ANSI escape codes on Windows (used for color, cursor control, etc.)
unsigned short enable_win_ansi();

/// Global variable used to toggle all logging
extern atomic_bool logging_enabled;

#endif // LOGGING_H

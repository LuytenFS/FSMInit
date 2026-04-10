#pragma once

#include <stdbool.h>

/* =====================================
   Colors
   ===================================== */

extern bool g_color_enabled;

#define COL_RESET (g_color_enabled ? "\033[0m" : "")

#define COL_BLACK (g_color_enabled ? "\033[30m" : "")
#define COL_RED (g_color_enabled ? "\033[31m" : "")
#define COL_GREEN (g_color_enabled ? "\033[32m" : "")
#define COL_YELLOW (g_color_enabled ? "\033[33m" : "")
#define COL_BLUE (g_color_enabled ? "\033[34m" : "")
#define COL_MAGENTA (g_color_enabled ? "\033[35m" : "")
#define COL_CYAN (g_color_enabled ? "\033[36m" : "")
#define COL_WHITE (g_color_enabled ? "\033[37m" : "")
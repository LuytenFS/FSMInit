#pragma once

#include <stdbool.h>

/* =====================================
   Text Types
   ===================================== */

extern bool g_color_enabled;

#define TEX_BOLD (g_color_enabled ? "\033[1m" : "")
#define TEX_DIM (g_color_enabled ? "\033[2m" : "")
#define TEX_ITALIC (g_color_enabled ? "\033[3m" : "")
#define TEX_UNDERLINE (g_color_enabled ? "\033[4m" : "")
#define TEX_BLINK (g_color_enabled ? "\033[5m" : "")
#define TEX_REVERSE (g_color_enabled ? "\033[7m" : "")
#define TEX_STRIKE (g_color_enabled ? "\033[9m" : "")
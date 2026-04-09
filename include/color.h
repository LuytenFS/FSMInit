#pragma once

extern bool g_color_enabled;

#define COL_RESET "\033[0m"

/* =====================================
   Colors
   ===================================== */

#define COL_BLACK "\033[30m"
#define COL_RED "\033[31m"
#define COL_GREEN "\033[32m"
#define COL_YELLOW "\033[33m"
#define COL_BLUE "\033[34m"
#define COL_MAGENTA "\033[35m"
#define COL_CYAN "\033[36m"
#define COL_WHITE "\033[37m"

// In color.h
#ifndef COLOR_ENABLED
extern bool g_color_enabled;
#define PRINTF_IF_COLOR(fmt, ...)        \
    do                                  \
    {                                   \
        if (g_color_enabled)            \
            printf(fmt, ##__VA_ARGS__); \
        else                            \
            printf(fmt, ##__VA_ARGS__); \
    } while (0)
    
#define FPRINT_IF_COLOR(stream, fmt, ...)        \
    do                                           \
    {                                            \
        if (g_color_enabled)                     \
            fprintf(stream, fmt, ##__VA_ARGS__); \
        else                                     \
            fprintf(stream, fmt, ##__VA_ARGS__); \
    } while (0)
#endif
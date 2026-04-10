#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#define isatty _isatty
#else
#include <unistd.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include "version.h"
#include "def_type.h"
#include "file_subsystem.h"
#include "text_type.h"
#include "color.h"

bool g_color_enabled = false;

int main(int argc, char *argv[])
{
    int tty = isatty(1);
    if (getenv("NO_COLOR") || getenv("CI"))
        tty = 0;
    g_color_enabled = tty;

    OP operation = {0};

    bool is_stdmc = (argc > 1 && strcmp(argv[1], "-stdmc") == 0);
    bool is_stdm = (argc > 1 && strcmp(argv[1], "-stdm") == 0);

    // -----------------------------
    // Check at least one argument
    // -----------------------------
    if (argc < 2)
    {
        fprintf(stderr, "%s%sUsage:%s <command> <path> <-tbl/-tbm> [prefix if -tbm] [optional -debug]\n",
                TEX_BOLD, COL_YELLOW, COL_RESET);
        return 1;
    }

    // -----------------------------
    // Initialize operation struct
    // -----------------------------
    operation.command = argv[1];
    operation.path = argc > 2 ? argv[2] : NULL;
    operation.table_type = is_stdmc && argc > 3 ? argv[3] : NULL;
    operation.prefix = NULL;
    operation.debug = 0;
    operation.dry_run = 0;

    // -----------------------------
    // Handle -help
    // -----------------------------
    if (strcmp(argv[1], "-help") == 0)
    {
        if (argc != 2)
        {
            fprintf(stderr, "%s%sError:%s The '-help' command cannot have additional arguments.\n",
                    TEX_BOLD, COL_RED, COL_RESET);
            return 1;
        }

        printf(
            "Command-line arguments for the program:\n\n"
            "1. <command>       : Allowed values:\n"
            "                     - \"-stdm\"   : Standard Mod (no tables)\n"
            "                     - \"-stdmc\"  : Standard Mod Complex (with tables)\n"
            "                     - \"-help\"   : Display usage information\n\n"
            "2. <path>          : Directory path where the tool will create directories and tables.\n\n"
            "3. <-tbl/-tbm>     : Specifies whether to create standard .tbl files or modular .tbm files (only for -stdmc).\n\n"
            "4. <prefix>        : Optional prefix for .tbm files. Required only if \"-tbm\" is specified.\n"
            "                     Ignored for \"-tbl\".\n\n"
            "5. [-debug]        : Optional flag. Enables debug output to \"log.txt\" in the program's current directory.\n"
            "6. [-dry-run]      : Optional flag. Runs a dry run, showing what would happen without doing it.\n");

        return 0;
    }

    if (strcmp(argv[1], "-version") == 0)
    {
        printf("Developed by LuytenKy, FSMInit Version: (" FSMINIT_VERSION ")\n");
        return 0;
    }

    // -----------------------------
    // Determine command type
    // -----------------------------
    if (!is_stdmc && !is_stdm)
    {
        fprintf(stderr, "%s%sError:%s Unknown command '%s'. Use -help for usage.\n",
                TEX_BOLD, COL_RED, COL_RESET, argv[1]);
        return 1;
    }

    // -----------------------------
    // Validate argument count
    // -----------------------------
    int min_args = is_stdm ? 3 : 4;
    if (argc < min_args)
    {
        if (is_stdm)
            fprintf(stderr, "%s%sUsage:%s <command> <path> [optional -debug]\n",
                    TEX_BOLD, COL_YELLOW, COL_RESET);
        else
            fprintf(stderr, "%s%sUsage:%s <command> <path> <-tbl/-tbm> [prefix if -tbm] [optional -debug]\n",
                    TEX_BOLD, COL_YELLOW, COL_RESET);
        return 1;
    }

    // -----------------------------
    // Buffer checks
    // -----------------------------
    if (strlen(argv[2]) >= PATH_MAX)
    {
        fprintf(stderr, "%s%sError:%s Path is too long. Maximum %d characters.\n",
                TEX_BOLD, COL_RED, COL_RESET, PATH_MAX - 1);
        return 1;
    }

    // -----------------------------
    // Handle optional prefix if -tbm
    // -----------------------------
    int start_index = is_stdm ? 3 : 4;
    if (is_stdmc && operation.table_type && strcmp(operation.table_type, "-tbm") == 0)
    {
        if (argc < 5)
        {
            fprintf(stderr, "%s%sError:%s '.tbm' file extension requires a prefix argument.\n",
                    TEX_BOLD, COL_RED, COL_RESET);
            return 1;
        }
        if (strlen(argv[4]) > 32)
        {
            fprintf(stderr, "%s%sError:%s Prefix is too long. Maximum 32 characters.\n",
                    TEX_BOLD, COL_RED, COL_RESET);
            return 1;
        }
        operation.prefix = argv[4];
        start_index = 5;
    }

    // -----------------------------
    // Handle optional -debug/-dry-run
    // -----------------------------
    for (int i = start_index; i < argc; ++i)
    {
        /* These cannot ever be null */
        if (strcmp(argv[i], "-debug") == 0) // NOLINT(clang-analyzer-core.NonNullParamChecker)
            operation.debug = 1;
        else if (strcmp(argv[i], "-dry-run") == 0) // NOLINT(clang-analyzer-core.NonNullParamChecker)
            operation.dry_run = 1;
    }

    // -----------------------------
    // Debug output if enabled
    // -----------------------------
    if (operation.debug)
    {
        FILE *log_file = fopen("log.txt", "a");
        if (log_file)
        {
            fprintf(log_file, "=== Debug Start ===\n");
            fprintf(log_file, "Command       : %s\n", operation.command);
            fprintf(log_file, "Path          : %s\n", operation.path);
            fprintf(log_file, "Table type    : %s\n", operation.table_type ? operation.table_type : "N/A");
            fprintf(log_file, "Prefix        : %s\n", operation.prefix ? operation.prefix : "N/A");
            fprintf(log_file, "Debug enabled : %d\n", operation.debug);
            fprintf(log_file, "Dry-run       : %s\n", operation.dry_run ? "enabled" : "disabled");
            fprintf(log_file, "===================\n\n");
            fclose(log_file);
        }
    }

    // -----------------------------
    // Execute based on command
    // -----------------------------
    if (check_if_mod_structure_exists(operation.path))
    {
        fprintf(stderr, "%s%sError:%s Mod structure already exists at: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation.path);
        return 1;
    }

    create_directories(&operation);

    if (is_stdmc)
    {
        create_static_tables(&operation);

        if (operation.table_type && strcmp(operation.table_type, "-tbm") == 0)
            create_modular_tables(&operation);
        else if (operation.table_type && strcmp(operation.table_type, "-tbl") != 0)
        {
            fprintf(stderr, "%s%sError:%s Unknown table type '%s'. Use -tbl or -tbm.\n",
                    TEX_BOLD, COL_RED, COL_RESET, operation.table_type);
            return 1;
        }
    }

    return 0;
}
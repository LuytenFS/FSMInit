#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "def_type.h"
#include "file_subsystem.h"

int main(int argc, char *argv[])
{
    // -----------------------------
    // Check at least one argument
    // -----------------------------
    if (argc < 2)
    {
        fprintf(stderr, "Usage: <command> <path> <-tbl/-tbm> [prefix if -tbm] [optional -debug]\n");
        return 1;
    }

    // -----------------------------
    // Handle -help
    // -----------------------------
    if (strcmp(argv[1], "-help") == 0)
    {
        if (argc != 2)
        {
            fprintf(stderr, "Error: The -help command cannot have additional arguments.\n");
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
            "5. [-debug]        : Optional flag. Enables debug output to \"log.txt\" in the program's current directory.\n");

        return 0;
    }

    // -----------------------------
    // Determine command type
    // -----------------------------
    bool is_stdmc = (strcmp(argv[1], "-stdmc") == 0);
    bool is_stdm = (strcmp(argv[1], "-stdm") == 0);

    if (!is_stdmc && !is_stdm)
    {
        fprintf(stderr, "Error: Unknown command '%s'. Use -help for usage.\n", argv[1]);
        return 1;
    }

    // -----------------------------
    // Validate argument count
    // -----------------------------
    int min_args = is_stdm ? 2 + 1 : 3 + 1; // command+path or command+path+table_type
    if (argc < min_args)
    {
        if (is_stdm)
            fprintf(stderr, "Usage: <command> <path> [optional -debug]\n");
        else
            fprintf(stderr, "Usage: <command> <path> <-tbl/-tbm> [prefix if -tbm] [optional -debug]\n");
        return 1;
    }

    // -----------------------------
    // Initialize operation struct
    // -----------------------------
    OP operation = {0};
    operation.command = argv[1];
    operation.path = argv[2];
    operation.table_type = is_stdmc ? argv[3] : NULL;
    operation.prefix = NULL;
    operation.debug = 0;

    // -----------------------------
    // Handle optional prefix if -tbm (only for -stdmc)
    // -----------------------------
    int start_index = is_stdm ? 3 : 4; // argv index where optional args start
    if (is_stdmc && strcmp(operation.table_type, "-tbm") == 0)
    {
        if (argc < 5)
        {
            fprintf(stderr, "Error: .tbm requires a prefix argument.\n");
            return 1;
        }
        operation.prefix = argv[4];
        start_index = 5; // optional args start after prefix
    }

    // -----------------------------
    // Handle optional -debug flag
    // -----------------------------
    for (int i = start_index; i < argc; ++i)
    {
        if (strcmp(argv[i], "-debug") == 0)
        {
            operation.debug = 1;
            break;
        }
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
            fprintf(log_file, "===================\n\n");
            fclose(log_file);
        }
    }

    // -----------------------------
    // Execute based on command
    // -----------------------------
    create_directories(&operation); // Pass pointer to operation

    if (is_stdmc)
    {
        if (strcmp(operation.table_type, "-tbl") == 0)
            create_static_tables(&operation);
        else if (strcmp(operation.table_type, "-tbm") == 0)
            create_modular_tables(&operation);
        else
        {
            fprintf(stderr, "Error: Unknown table type '%s'. Use -tbl or -tbm.\n", operation.table_type);
            return 1;
        }
    }

    return 0;
}
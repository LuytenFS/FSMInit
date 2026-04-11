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

#include "version.h"
#include "def_type.h"
#include "file_subsystem.h"
#include "text_type.h"
#include "color.h"
#include "boilerplate_subsystem.h"

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

    if (argc < 2)
    {
        fprintf(stderr, "%s%sUsage:%s <command> <path> <-tbl/-tbm> [prefix if -tbm] [optional -debug/-dry-run]\n",
                TEX_BOLD, COL_YELLOW, COL_RESET);
        return 1;
    }

    operation.command = argv[1];
    operation.path = argc > 2 ? argv[2] : NULL;
    operation.table_type = is_stdmc && argc > 3 ? argv[3] : NULL;
    operation.prefix = NULL;
    operation.debug = 0;
    operation.dry_run = 0;
    operation.gen_boilerplate = 0;

    if (strcmp(argv[1], "-help") == 0)
    {
        if (argc != 2)
        {
            fprintf(stderr, "%s%sError:%s The '-help' command cannot have additional arguments.\n",
                    TEX_BOLD, COL_RED, COL_RESET);
            return 1;
        }

        printf(
            "Command-line arguments:\n\n"
            "1. <command>     : \"-stdm\" (dirs only), \"-stdmc\" (dirs+tables), \"-help\", \"-version\"\n"
            "2. <path>        : Target directory\n"
            "3. <-tbl/-tbm>   : Table type (stdmc only)\n"
            "4. <prefix>      : Required for -tbm\n"
            "5. [-debug]      : Log to log.txt\n"
            "6. [-dry-run]    : Simulate only\n");
        return 0;
    }

    if (strcmp(argv[1], "-version") == 0)
    {
        printf("Developed by LuytenKy, FSMInit Version: (" FSMINIT_VERSION ")\n");
        return 0;
    }

    if (!is_stdmc && !is_stdm)
    {
        fprintf(stderr, "%s%sError:%s Unknown command '%s'. Use -help.\n",
                TEX_BOLD, COL_RED, COL_RESET, argv[1]);
        return 1;
    }

    int min_args = is_stdm ? 3 : 4;
    if (argc < min_args)
    {
        if (is_stdm)
            fprintf(stderr, "%s%sUsage:%s -stdm <path> [-debug] [-dry-run]\n",
                    TEX_BOLD, COL_YELLOW, COL_RESET);
        else
            fprintf(stderr, "%s%sUsage:%s -stdmc <path> <-tbl/-tbm> [prefix] [-debug] [-dry-run]\n",
                    TEX_BOLD, COL_YELLOW, COL_RESET);
        return 1;
    }

    if (strlen(argv[2]) >= PATH_MAX)
    {
        fprintf(stderr, "%s%sError:%s Path too long (max %d chars)\n",
                TEX_BOLD, COL_RED, COL_RESET, PATH_MAX - 1);
        return 1;
    }

    if (is_stdmc && operation.table_type && strcmp(operation.table_type, "-tbm") == 0)
    {
        if (argc < 5)
        {
            fprintf(stderr, "%s%sError:%s -tbm requires prefix\n", TEX_BOLD, COL_RED, COL_RESET);
            return 1;
        }
        if (strlen(argv[4]) > 32)
        {
            fprintf(stderr, "%s%sError:%s Prefix too long (max 32 chars)\n", TEX_BOLD, COL_RED, COL_RESET);
            return 1;
        }
        operation.prefix = argv[4];
    }

    int arg_idx = is_stdm ? 3 : 4;
    for (int i = arg_idx; i < argc; ++i)
    {
        if (strcmp(argv[i], "-bpl") == 0)
        {
            operation.gen_boilerplate = 1;
        }
        else if (!operation.table_type &&
                 (strcmp(argv[i], "-tbl") == 0 || strcmp(argv[i], "-tbm") == 0))
        {
            operation.table_type = argv[i];
        }
        else if (operation.table_type && strcmp(operation.table_type, "-tbm") == 0 &&
                 !operation.prefix)
        {
            if (strlen(argv[i]) > 32)
            {
                fprintf(stderr, "%s%sError:%s Prefix too long (max 32 chars)\n",
                        TEX_BOLD, COL_RED, COL_RESET);
                return 1;
                return 1;
            }
            operation.prefix = argv[i];
        }
        else if (strcmp(argv[i], "-debug") == 0)
        {
            operation.debug = 1;
        }
        else if (strcmp(argv[i], "-dry-run") == 0)
        {
            operation.dry_run = 1;
        }
        else
        {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return 1;
        }
    }

    if (operation.debug)
    {
        FILE *log_file = fopen("log.txt", "a");
        if (log_file)
        {
            const char *table_str = operation.table_type ? operation.table_type : "N/A";
            const char *prefix_str = operation.prefix ? operation.prefix : "N/A";
            fprintf(log_file, "=== Debug ===\nCommand: %s\nPath: %s\nTable: %s\nPrefix: %s\nDebug: %d\nDry-run: %d\n===========\n\n",
                    operation.command, operation.path, table_str, prefix_str,
                    operation.debug, operation.dry_run);
            fclose(log_file);
        }
    }

    if (check_if_mod_structure_exists(operation.path))
    {
        fprintf(stderr, "%s%sError:%s Mod structure exists at: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation.path);
        return 1;
    }

    create_directories(&operation);

    if (is_stdmc)
    {
        create_static_tables(&operation);

        if (operation.table_type)
        {
            if (strcmp(operation.table_type, "-tbl") == 0)
                create_tbl_tables(&operation);
            else if (strcmp(operation.table_type, "-tbm") == 0)
                create_tbm_tables(&operation);
            else
            {
                fprintf(stderr, "%s%sError:%s Unknown table type '%s' (use -tbl or -tbm)\n",
                        TEX_BOLD, COL_RED, COL_RESET, operation.table_type);
                return 1;
            }
        }
    }

    if (operation.table_type && operation.gen_boilerplate)
    {
        printf("%sWriting boilerplate to tables...%s%s\n", TEX_BOLD, COL_MAGENTA, COL_RESET);

        char *tables_path = NULL;
        if (asprintf(&tables_path, "%s/tables", operation.path) == -1)
        {
            operation.errors++;
        }
        else
        {
            TABLE_FILE_LIST *tables = verify_tables_directory(tables_path);
            if (tables)
            {
                for (size_t i = 0; i < tables->count; i++)
                {
                    const BPL_ENTRY *entry = find_boilerplate(tables->paths[i]);
                    if (entry)
                    {
                        write_to_tables(entry, tables->paths[i], &operation);
                    }
                }
                // Cleanup
                for (size_t i = 0; i < tables->count; i++)
                    free(tables->paths[i]);
                free(tables->paths);
                free(tables);
            }
            free(tables_path);
        }
    }

    printf("\n%s%s--- Summary ---%s\n", TEX_BOLD, COL_CYAN, COL_RESET);
    printf("  %sDirectories:%s %s%d%s\n", TEX_BOLD, COL_RESET, COL_GREEN, operation.dirs_created, COL_RESET);
    printf("  %sTables:%s     %s%d%s\n", TEX_BOLD, COL_RESET, COL_GREEN, operation.tables_created, COL_RESET);
    printf("  %sErrors:%s     ", TEX_BOLD, COL_RESET);
    if (operation.errors > 0)
    {
        printf("%s%d%s\n", COL_RED, operation.errors, COL_RESET);
    }
    else
    {
        printf("%s%d%s\n", COL_GREEN, operation.errors, COL_RESET);
    }
    printf("%s%s----------------%s\n", TEX_BOLD, COL_CYAN, COL_RESET);

    if (operation.debug)
    {
        FILE *log_file = fopen("log.txt", "a");
        if (log_file)
        {
            fprintf(log_file, "Summary: Dirs=%d, Tables=%d, Errors=%d\n\n",
                    operation.dirs_created, operation.tables_created, operation.errors);
            fclose(log_file);
        }
    }

    return 0;
}
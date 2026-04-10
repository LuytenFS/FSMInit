/* file_subsystem.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#include <io.h>
#define check_writable(p) (_access((p), 2) == 0)
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#define S_ISDIR(m) (((m) & _S_IFDIR) != 0)
#define asprintf(buf, fmt, ...) _asprintf(buf, fmt, __VA_ARGS__)
static int _asprintf(char **ret, const char *format, ...)
{
    va_list ap;
    int size;
    va_start(ap, format);
    size = vsnprintf(NULL, 0, format, ap);
    va_end(ap);
    if (size < 0)
        return -1;
    *ret = malloc(size + 1);
    if (!*ret)
        return -1;
    va_start(ap, format);
    size = vsnprintf(*ret, size + 1, format, ap);
    va_end(ap);
    return size;
}
#else
#include <unistd.h>
#define check_writable(p) (access((p), W_OK) == 0)
#endif

#include "def_type.h"
#include "color.h"
#include "text_type.h"

/* =====================================
   Voice Subdirectories
   ===================================== */
const char *voice_subdirs[] = {
    "briefing",
    "command_briefings",
    "debriefing",
    "personas",
    "special",
    "training"};

const size_t voice_subdirs_count = 6;

/* =====================================
   Directory Table
   ===================================== */
FS_DIRECTORIES fs_dirs[] = {
    {"cache", NULL, 0},
    {"cbanims", NULL, 0},
    {"config", NULL, 0},
    {"demos", NULL, 0},
    {"effects", NULL, 0},
    {"fonts", NULL, 0},
    {"force feedback", NULL, 0},
    {"freddocs", NULL, 0},
    {"hud", NULL, 0},
    {"intelanims", NULL, 0},
    {"interface", NULL, 0},
    {"maps", NULL, 0},
    {"missions", NULL, 0},
    {"models", NULL, 0},
    {"movies", NULL, 0},
    {"multidata", NULL, 0},
    {"music", NULL, 0},
    {"players", NULL, 0},
    {"images", NULL, 0},
    {"multi", NULL, 0},
    {"single", NULL, 0},
    {"squads", NULL, 0},
    {"scripts", NULL, 0},
    {"sounds", NULL, 0},
    {"8b22k", NULL, 0},
    {"16b11k", NULL, 0},
    {"tables", NULL, 0},
    {"text", NULL, 0},
    {"voice", (const char **)voice_subdirs, 6}};

const size_t fs_dirs_count = 29;

/* =====================================
   Modular Tables (.tbl / .tbm)
   ===================================== */
FS_TABLE_ENTRY fs_tables[] = {
    {"ai", "tbl", true, "-aic"},
    {"ai_profiles", "tbl", true, "-aip"},
    {"armor", "tbl", false, "-amr"},
    {"asteroid", "tbl", true, "-ast"},
    {"autopilot", "tbl", false, "-aplt"},
    {"cheats", "tbl", false, "-cht"},
    {"colors", "tbl", false, "-clr"},
    {"credits", "tbl", true, "-crd"},
    {"cutscenes", "tbl", true, "-csn"},
    {"fireball", "tbl", false, "-fbl"},
    {"fonts", "tbl", true, "-fnt"},
    {"game_settings", "tbl", false, "-mod"},
    {"glowpoints", "tbl", false, "-gpo"},
    {"help", "tbl", false, "-hlp"},
    {"hud_gauges", "tbl", false, "-hdg"},
    {"iff_defs", "tbl", true, "-iff"},
    {"lightning", "tbl", true, "-ltng"},
    {"mainhall", "tbl", false, "-hall"},
    {"medals", "tbl", true, "-mdl"},
    {"messages", "tbl", false, "-msg"},
    {"mflash", "tbl", false, "-mfl"},
    {"music", "tbl", true, "-mus"},
    {"nebula", "tbl", true, "-neb"},
    {"objecttypes", "tbl", true, "-obt"},
    {"rank", "tbl", true, "-rnk"},
    {"scripting", "tbl", false, "-sct"},
    {"ships", "tbl", true, "-shp"},
    {"sounds", "tbl", true, "-snd"},
    {"species_defs", "tbl", true, "-sdf"},
    {"species", "tbl", true, "-intl"},
    {"ssm", "tbl", false, "-ssm"},
    {"stars", "tbl", false, "-str"},
    {"strings", "tbl", false, "-lcl"},
    {"tips", "tbl", true, "-tip"},
    {"traitor", "tbl", false, "-trtr"},
    {"tstrings", "tbl", false, "-tlc"},
    {"weapons", "tbl", true, "-wep"},
    {"weapon_expl", "tbl", false, "-wxp"},
    {"particle_effects", "tbl", true, "-part"}};

const size_t fs_tables_count = 39;

/* =====================================
   Static Tables (non-modular)
   ===================================== */
FS_TABLES static_tables[] = {
    {"Controlconfigdefaults.tbl"}};

const size_t static_tables_count = 1;

/* =====================================
   File & Directory Creation
   ===================================== */

static bool path_is_dir(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return false;
    return S_ISDIR(st.st_mode);
}

bool check_if_mod_structure_exists(const char *base_path)
{
    for (size_t i = 0; i < fs_dirs_count; i++)
    {
        char *dir_path = NULL;
        if (asprintf(&dir_path, "%s/%s", base_path, fs_dirs[i].name) == -1)
            continue;

        if (!path_is_dir(dir_path))
        {
            free(dir_path);
            return false;
        }

        for (size_t j = 0; j < fs_dirs[i].subdir_count; j++)
        {
            char *subdir_path = NULL;
            if (asprintf(&subdir_path, "%s/%s", dir_path, fs_dirs[i].subdirs[j]) == -1)
            {
                free(dir_path);
                return false;
            }

            if (!path_is_dir(subdir_path))
            {
                free(subdir_path);
                free(dir_path);
                return false;
            }

            free(subdir_path);
        }

        free(dir_path);
    }
    return true;
}

static void ensure_tables_dir(OP *operation)
{
    FILE *log = operation->debug ? fopen("log.txt", "a") : NULL;
    char *tables_path = NULL;
    if (asprintf(&tables_path, "%s/tables", operation->path) == -1)
    {
        if (log)
            fclose(log);
        return;
    }

    if (!operation->dry_run)
    {
        int mkdir_result = mkdir(tables_path, 0755);
        if (mkdir_result != 0 && errno != EEXIST)
        {
            if (log)
                fprintf(log, "Error: Failed to create tables directory: %s - %s\n", tables_path, strerror(errno));
            fprintf(stderr, "%s%sError:%s Failed to create tables directory: %s - %s\n",
                    TEX_BOLD, COL_RED, COL_RESET, tables_path, strerror(errno));
            operation->errors++;
        }
        else if (mkdir_result == 0)
        {
            if (log)
                fprintf(log, "Success: Created tables directory: %s\n", tables_path);
            printf("%s%sSuccess:%s Created tables directory: %s\n",
                   TEX_BOLD, COL_GREEN, COL_RESET, tables_path);
            operation->dirs_created++;
        }
    }
    else
    {
        printf("%s[dry-run]%s Would create tables directory: %s\n", COL_YELLOW, COL_RESET, tables_path);
        operation->dirs_created++;
    }

    free(tables_path);
    if (log)
        fclose(log);
}

static void create_table_file(OP *operation, const char *filename, FILE *log, const char *type)
{
    if (operation->dry_run)
    {
        if (log)
            fprintf(log, "Dry-run: Would create %s file: %s\n", type, filename);
        printf("%s[dry-run]%s Would create %s file: %s\n", COL_YELLOW, COL_RESET, type, filename);
        operation->tables_created++;
    }
    else
    {
        FILE *f = fopen(filename, "w");
        if (f)
        {
            fclose(f);
            if (log)
                fprintf(log, "Success: Created %s file: %s\n", type, filename);
            printf("%s%sSuccess:%s Created %s file: %s\n",
                   TEX_BOLD, COL_GREEN, COL_RESET, type, filename);
            operation->tables_created++;
        }
        else
        {
            if (log)
                fprintf(log, "Error: Failed to create %s file: %s - %s\n", type, filename, strerror(errno));
            fprintf(stderr, "%s%sError:%s Failed to create %s file: %s - %s\n",
                    TEX_BOLD, COL_RED, COL_RESET, type, filename, strerror(errno));
            operation->errors++;
        }
    }
}

void create_directories(OP *operation)
{
    struct stat st;
    FILE *log = operation->debug ? fopen("log.txt", "a") : NULL;

    if (!operation || !operation->path || operation->path[0] == '\0')
    {
        if (log)
            fclose(log);
        return;
    }

    if (stat(operation->path, &st) != 0 || !S_ISDIR(st.st_mode))
    {
        if (log)
            fprintf(log, "Error: Target path does not exist or is not a directory: %s\n", operation->path);
        fprintf(stderr, "%s%sError:%s Target path does not exist or is not a directory: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation->path);
        operation->errors++;
        if (log)
            fclose(log);
        return;
    }

    if (!check_writable(operation->path))
    {
        if (log)
            fprintf(log, "Error: Target path is not writable: %s\n", operation->path);
        fprintf(stderr, "%s%sError:%s Target path is not writable: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation->path);
        operation->errors++;
        if (log)
            fclose(log);
        return;
    }

    for (size_t i = 0; i < fs_dirs_count; i++)
    {
        char *dir_path = NULL;
        if (asprintf(&dir_path, "%s/%s", operation->path, fs_dirs[i].name) == -1)
            continue;

        if (operation->dry_run)
        {
            if (log)
                fprintf(log, "Dry-run: Would create directory: %s\n", dir_path);
            printf("%s[dry-run]%s Would create directory: %s\n", COL_YELLOW, COL_RESET, dir_path);
            operation->dirs_created++;
        }
        else
        {
            int mkdir_result = mkdir(dir_path, 0755);
            if (mkdir_result != 0 && errno != EEXIST)
            {
                if (log)
                    fprintf(log, "Error: Failed to create directory: %s - %s\n", dir_path, strerror(errno));
                fprintf(stderr, "%s%sError:%s Failed to create directory: %s - %s\n",
                        TEX_BOLD, COL_RED, COL_RESET, dir_path, strerror(errno));
                operation->errors++;
            }
            else if (mkdir_result == 0)
            {
                if (log)
                    fprintf(log, "Success: Created directory: %s\n", dir_path);
                printf("%s%sSuccess:%s Created directory: %s\n", TEX_BOLD, COL_GREEN, COL_RESET, dir_path);
                operation->dirs_created++;
            }
        }

        for (size_t j = 0; j < fs_dirs[i].subdir_count; j++)
        {
            char *subdir_path = NULL;
            if (asprintf(&subdir_path, "%s/%s", dir_path, fs_dirs[i].subdirs[j]) == -1)
            {
                free(dir_path);
                continue;
            }

            if (operation->dry_run)
            {
                if (log)
                    fprintf(log, "Dry-run: Would create subdirectory: %s\n", subdir_path);
                printf("%s[dry-run]%s Would create subdirectory: %s\n", COL_YELLOW, COL_RESET, subdir_path);
                operation->dirs_created++;
            }
            else
            {
                int sub_mkdir_result = mkdir(subdir_path, 0755);
                if (sub_mkdir_result != 0 && errno != EEXIST)
                {
                    if (log)
                        fprintf(log, "Error: Failed to create subdirectory: %s - %s\n", subdir_path, strerror(errno));
                    fprintf(stderr, "%s%sError:%s Failed to create subdirectory: %s - %s\n",
                            TEX_BOLD, COL_RED, COL_RESET, subdir_path, strerror(errno));
                    operation->errors++;
                }
                else if (sub_mkdir_result == 0)
                {
                    if (log)
                        fprintf(log, "Success: Created subdirectory: %s\n", subdir_path);
                    printf("%s%sSuccess:%s Created subdirectory: %s\n", TEX_BOLD, COL_GREEN, COL_RESET, subdir_path);
                    operation->dirs_created++;
                }
            }

            free(subdir_path);
        }
        free(dir_path);
    }

    if (log)
        fclose(log);
}

void create_tbl_tables(OP *operation)
{
    struct stat st;
    FILE *log = operation->debug ? fopen("log.txt", "a") : NULL;

    if (!operation || !operation->path || operation->path[0] == '\0')
    {
        if (log)
            fclose(log);
        return;
    }

    if (stat(operation->path, &st) != 0 || !S_ISDIR(st.st_mode))
    {
        if (log)
            fprintf(log, "Error: Target path does not exist or is not a directory: %s\n", operation->path);
        fprintf(stderr, "%s%sError:%s Target path does not exist or is not a directory: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation->path);
        operation->errors++;
        if (log)
            fclose(log);
        return;
    }

    if (!check_writable(operation->path))
    {
        if (log)
            fprintf(log, "Error: Target path is not writable: %s\n", operation->path);
        fprintf(stderr, "%s%sError:%s Target path is not writable: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation->path);
        operation->errors++;
        if (log)
            fclose(log);
        return;
    }

    ensure_tables_dir(operation);

    char *tables_path = NULL;
    if (asprintf(&tables_path, "%s/tables", operation->path) == -1)
    {
        if (log)
            fclose(log);
        return;
    }

    for (size_t i = 0; i < fs_tables_count; i++)
    {
        if (!fs_tables[i].is_modular)
            continue;

        char *filename = NULL;
        if (asprintf(&filename, "%s/%s.tbl", tables_path, fs_tables[i].base_name) == -1)
            continue;

        create_table_file(operation, filename, log, "table");
        free(filename);
    }

    free(tables_path);
    if (log)
        fclose(log);
}

void create_tbm_tables(OP *operation)
{
    struct stat st;
    FILE *log = operation->debug ? fopen("log.txt", "a") : NULL;

    if (!operation || !operation->path || !operation->prefix || operation->prefix[0] == '\0')
    {
        if (log)
            fprintf(log, "Error: Missing prefix for TBM\n");
        fprintf(stderr, "%s%sError:%s TBM requires prefix\n", TEX_BOLD, COL_RED, COL_RESET);
        operation->errors++;
        if (log)
            fclose(log);
        return;
    }

    if (stat(operation->path, &st) != 0 || !S_ISDIR(st.st_mode))
    {
        if (log)
            fprintf(log, "Error: Target path does not exist or is not a directory: %s\n", operation->path);
        fprintf(stderr, "%s%sError:%s Target path does not exist or is not a directory: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation->path);
        operation->errors++;
        if (log)
            fclose(log);
        return;
    }

    if (!check_writable(operation->path))
    {
        if (log)
            fprintf(log, "Error: Target path is not writable: %s\n", operation->path);
        fprintf(stderr, "%s%sError:%s Target path is not writable: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation->path);
        operation->errors++;
        if (log)
            fclose(log);
        return;
    }

    ensure_tables_dir(operation);

    char *tables_path = NULL;
    if (asprintf(&tables_path, "%s/tables", operation->path) == -1)
    {
        if (log)
            fclose(log);
        return;
    }

    for (size_t i = 0; i < fs_tables_count; i++)
    {
        if (!fs_tables[i].is_modular)
            continue;

        char *filename = NULL;
        if (asprintf(&filename, "%s/%s%s.tbm", tables_path, operation->prefix, fs_tables[i].modular_suffix) == -1)
            continue;

        create_table_file(operation, filename, log, "TBM table");
        free(filename);
    }

    free(tables_path);
    if (log)
        fclose(log);
}

void create_static_tables(OP *operation)
{
    struct stat st;
    FILE *log = operation->debug ? fopen("log.txt", "a") : NULL;

    if (!operation || !operation->path || operation->path[0] == '\0')
    {
        if (log)
            fclose(log);
        return;
    }

    if (stat(operation->path, &st) != 0 || !S_ISDIR(st.st_mode))
    {
        if (log)
            fprintf(log, "Error: Target path does not exist or is not a directory: %s\n", operation->path);
        fprintf(stderr, "%s%sError:%s Target path does not exist or is not a directory: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation->path);
        operation->errors++;
        if (log)
            fclose(log);
        return;
    }

    if (!check_writable(operation->path))
    {
        if (log)
            fprintf(log, "Error: Target path is not writable: %s\n", operation->path);
        fprintf(stderr, "%s%sError:%s Target path is not writable: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation->path);
        operation->errors++;
        if (log)
            fclose(log);
        return;
    }

    ensure_tables_dir(operation);

    char *tables_path = NULL;
    if (asprintf(&tables_path, "%s/tables", operation->path) == -1)
    {
        if (log)
            fclose(log);
        return;
    }

    for (size_t i = 0; i < static_tables_count; i++)
    {
        char *filename = NULL;
        if (asprintf(&filename, "%s/%s", tables_path, static_tables[i].name) == -1)
            continue;

        create_table_file(operation, filename, log, "static table");
        free(filename);
    }

    free(tables_path);
    if (log)
        fclose(log);
}
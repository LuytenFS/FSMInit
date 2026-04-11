#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#ifdef _WIN32
#include <io.h>
#define check_writable(p) (_access((p), 2) == 0)
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
#include "file_subsystem.h"
#include "boilerplate_subsystem.h"

/*
check to see if the mod structure has generated successfuly, if so,
!!!(for now discard all of the unneeded data and focus only on the tables/ being present)!!!
*/
bool verify_mod_structure(const char *base_path)
{
    if (path_is_dir(base_path))
    {
        for (size_t i = 0; i < fs_dirs_count; i++)
        {
            char *dir_path = NULL;
            if (asprintf(&dir_path, "%s/%s", base_path, fs_dirs[i].name) == -1)
                return false;

            bool exists = path_is_dir(dir_path);
            free(dir_path);
            if(!exists)
            {
                return false;
            }
        }
        return true;
    } 
    else
    {
        return false;
    }
}

/* Verify and collect the tables from the "tables/" directory and return them */
TABLE_FILE_LIST *verify_tables_directory(const char *base_path_tables)
{
    TABLE_FILE_LIST *tfl = malloc(sizeof(TABLE_FILE_LIST));
    if (!tfl)
        return NULL;

    tfl->paths = NULL;
    tfl->count = 0;

    if (!path_is_dir(base_path_tables))
    {
        free(tfl);
        return NULL;
    }

    DIR *dir = opendir(base_path_tables);
    if (!dir)
    {
        perror("opendir");
        free(tfl);
        return NULL;
    }

    struct dirent *entry;
    char full_path[PATH_MAX];
    size_t capacity = 16;

    tfl->paths = malloc(capacity * sizeof(char *));
    if (!tfl->paths)
    {
        closedir(dir);
        free(tfl);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        struct stat st;
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path_tables, entry->d_name);
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode))
        {
            if (tfl->count >= capacity)
            {
                capacity *= 2;
                char **new_paths = realloc(tfl->paths, capacity * sizeof(char *));
                if (!new_paths)
                {
                    for (size_t j = 0; j < tfl->count; j++)
                        free(tfl->paths[j]);
                    free(tfl->paths);
                    closedir(dir);
                    free(tfl);
                    return NULL;
                }
                tfl->paths = new_paths;
            }

            tfl->paths[tfl->count] = strdup(full_path);
            if (!tfl->paths[tfl->count])
            {
                for (size_t j = 0; j < tfl->count; j++)
                    free(tfl->paths[j]);
                free(tfl->paths);
                closedir(dir);
                free(tfl);
                return NULL;
            }
            tfl->count++;
        }
    }

    closedir(dir);
    return tfl;
}

static char *strip_path_ext(const char *filename)
{
    const char *base = strrchr(filename, '/');
    if (!base)
        base = filename;
    else
        base++;

    char *dot = strrchr(base, '.');
    if (!dot)
        dot = base + strlen(base);

    size_t len = dot - base;
    char *key = malloc(len + 1);
    if (key)
    {
        strncpy(key, base, len);
        key[len] = '\0';
    }
    return key;
}

/* strip path and extension, extract key */
/* linear search through bpl_table[] */
/* return matching entry or NULL if not found */
const BPL_ENTRY *find_boilerplate(const char *filename)
{
    char *key = strip_path_ext(filename);
    if (!key)
        return NULL;

    for (size_t i = 0; i < bpl_table_count; i++)
    {
        if (strcmp(key, bpl_table[i].key) == 0)
        {
            free(key);
            return &bpl_table[i];
        }

        /* Check if key ends with the suffix (for .tbm files) */
        size_t suffix_len = strlen(bpl_table[i].key);
        size_t key_len = strlen(key);

        if (key_len >= suffix_len &&
            strcmp(key + key_len - suffix_len, bpl_table[i].key) == 0)
        {
            free(key);
            return &bpl_table[i];
        }
    }

    free(key);
    return NULL;
}
void write_to_tables(const BPL_ENTRY *entry, const char *filepath, OP *operation)
{
    FILE *fp;

    if(!check_writable(filepath))
    {
        fprintf(stderr, "%s%sError:%s Target path is not writable: %s\n",
                TEX_BOLD, COL_RED, COL_RESET, operation->path);
        return;
    }
    fp = fopen(filepath, "w");
    if(!fp)
    {
        perror("fopen");
        return;
    }

    /*
     *   This is where we will write to tables using their file paths
     *   since BPL_ENTRY's bpl_table[] has not yet been implemented, this function will remain as is.
     */

    fclose(fp);
}
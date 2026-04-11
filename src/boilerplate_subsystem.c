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

/*
check to see if the mod structure has generated successfuly, if so,
!!!(for now discard all of the unneeded data and focus only on the tables/ being present)!!!
*/
bool verify_mod_structure(const char *base_path)
{
    if(is_dir_path(base_path))
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

TABLE_FILE_LIST *verify_tables_directory(const char *base_path_tables)
{
    /* Verify and collect the tables from the "tables/" directory and return them */
}

const BPL_ENTRY *find_boilderplate(const char *filename)
{
    /* strip path and extension, extract key */
    /* linear search through bpl_table[] */
    /* return matching entry or NULL if not found */
}

void write_to_tables(const BPL_ENTRY *entry, const char *filepath, OP *operation)
{

}
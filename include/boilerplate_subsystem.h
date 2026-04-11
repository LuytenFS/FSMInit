#pragma once

#include "def_type.h"

/* Boilerplate string literals. Use macros so they remain constant expressions. */
#define BPL_WEP_PRIMARY    \
    "#Primary Weapons\n"   \
    "$Name: placeholder\n" \
    "#End\n"

/* Helper array using the macro directly (no variable in the initializer) */
static const char *bpl_wep_primary_variants[] = {
    BPL_WEP_PRIMARY};

/* Boilerplate table — key matches base_name for .tbl or suffix for .tbm */
static const BPL_ENTRY bpl_table[] = {
    {"weapons", bpl_wep_primary_variants, 1}};

static const size_t bpl_table_count = sizeof(bpl_table) / sizeof(bpl_table[0]);

/* Public functions */
bool verify_mod_structure(const char *base_path);
TABLE_FILE_LIST *verify_tables_directory(const char *base_path_tables);
const BPL_ENTRY *find_boilerplate(const char *filename);
void write_to_tables(const BPL_ENTRY *entry, const char *filepath, OP *operation);
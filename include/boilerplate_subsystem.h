#pragma once

#include "def_type.h"

/* Boilerplate string literals */
static const char *bpl_wep_primary =
    "#Primary Weapons\n"
    "$Name: placeholder\n"
    "#End\n";

/* Boilerplate table — key matches base_name for .tbl or suffix for .tbm */
static const BPL_ENTRY bpl_table[] = {
    {"weapons", (const char *[]){bpl_wep_primary}, 1},
};

static const size_t bpl_table_count = 1;
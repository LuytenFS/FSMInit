#pragma once

#include "def_type.h"

void create_directories(OP *operation);
void create_tbl_tables(OP *operation);
void create_tbm_tables(OP *operation);
void create_static_tables(OP *operation);
bool check_if_mod_structure_exists(const char *base_path);
bool path_is_dir(const char *path);
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#include "def_type.h"

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

const size_t voice_subdirs_count = sizeof(voice_subdirs) / sizeof(voice_subdirs[0]);

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
    {"voice", voice_subdirs, sizeof(voice_subdirs) / sizeof(voice_subdirs[0])}};

const size_t fs_dirs_count = sizeof(fs_dirs) / sizeof(fs_dirs[0]);

/* =====================================
   Modular Tables (.tbl / .tbm)
   ===================================== */
FS_TABLE_ENTRY fs_tables[] = {
    {"ai", "tbl", true},
    {"ai_profiles", "tbl", true},
    {"armor", "tbl", false},
    {"asteroid", "tbl", true},
    {"autopilot", "tbl", false},
    {"cheats", "tbl", false},
    {"colors", "tbl", false},
    {"credits", "tbl", true},
    {"cutscenes", "tbl", true},
    {"fireball", "tbl", false},
    {"fonts", "tbl", true},
    {"game_settings", "tbl", false},
    {"glowpoints", "tbl", false},
    {"help", "tbl", false},
    {"hud_gauges", "tbl", false},
    {"iff_defs", "tbl", true},
    {"lightning", "tbl", true},
    {"mainhall", "tbl", false},
    {"medals", "tbl", true},
    {"messages", "tbl", false},
    {"mflash", "tbl", false},
    {"music", "tbl", true},
    {"nebula", "tbl", true},
    {"objecttypes", "tbl", true},
    {"rank", "tbl", true},
    {"scripting", "tbl", false},
    {"ships", "tbl", true},
    {"sounds", "tbl", true},
    {"species_defs", "tbl", true},
    {"species", "tbl", true},
    {"ssm", "tbl", false},
    {"stars", "tbl", false},
    {"strings", "tbl", false},
    {"tips", "tbl", true},
    {"traitor", "tbl", false},
    {"tstrings", "tbl", false},
    {"weapons", "tbl", true},
    {"weapon_expl", "tbl", false},
    {"particle_effects", "tbl", true} // special XMT-only case
};

const size_t fs_tables_count = sizeof(fs_tables) / sizeof(fs_tables[0]);

/* =====================================
   Static Tables (non-modular)
   ===================================== */
FS_TABLES static_tables[] = {
    {"Ai_profiles.tbl"},
    {"Autopilot.tbl"},
    {"Colors.tbl"},
    {"Iff_defs.tbl"},
    {"Objecttypes.tbl"},
    {"Species_defs.tbl"},
    {"Armor.tbl"},
    {"Controlconfigdefaults.tbl"},
    {"Scripting.tbl"}};

const size_t static_tables_count = sizeof(static_tables) / sizeof(static_tables[0]);

/* =====================================
   File & Directory Creation
   ===================================== */

void create_directories(const char* base_path)
{
    for(size_t i = 0; i < fs_dirs_count; i++)
    {
        char dir_path[4096];
        snprintf(dir_path, sizeof(dir_path), "%s/%s", base_path, dir_path);
        if (mkdir(dir_path, 0755) != 0 && errno != EEXIST)
        {
            perror(dir_path);
            continue;
        }
        for (size_t j = 0; j < fs_dirs[i].subdir_count; ++j){
            char subdir_path[1024];
            snprintf(dir_path, sizeof(dir_path), "%s/%s", dir_path, fs_dirs[i].subdirs[j]);
            if(mkdir(subdir_path, 0755) != 0 && errno != EEXIST){
                perror(subdir_path);
            }
        }
    }
}

/* Assumes fs_tables[], static_tables[], and fs_tables_count/static_tables_count exist */

void create_modular_tables(const OP *operation)
{
    char tables_path[4096];
    snprintf(tables_path, sizeof(tables_path), "%s/tables", operation->path);

    // Ensure the 'tables/' directory exists
    if (mkdir(tables_path, 0755) != 0 && errno != EEXIST)
    {
        if (operation->debug)
            perror(tables_path);
    }

    for (size_t i = 0; i < fs_tables_count; ++i)
    {
        const FS_TABLE_ENTRY *entry = &fs_tables[i];
        char filename[4096];

        // Determine file extension: .tbm for modular + -tbm, otherwise .tbl
        const char *ext = (entry->is_modular && strcmp(operation->table_type, "-tbm") == 0) ? "tbm" : "tbl";

        if (strcmp(operation->table_type, "-tbm") == 0 && entry->is_modular && operation->prefix)
        {
            snprintf(filename, sizeof(filename), "%s/%s-%s.%s", tables_path, operation->prefix, entry->base_name, ext);
        }
        else
        {
            snprintf(filename, sizeof(filename), "%s/%s.%s", tables_path, entry->base_name, ext);
        }

        FILE *f = fopen(filename, "w");
        if (f)
        {
            fclose(f); // Close immediately after creating an empty file
        }
        else if (operation->debug)
        {
            perror(filename); // Print error if debug is enabled
        }
    }
}

void create_static_tables(const OP *operation)
{
    char tables_path[4096];
    snprintf(tables_path, sizeof(tables_path), "%s/tables", operation->path);

    // Ensure the 'tables/' directory exists
    if (mkdir(tables_path, 0755) != 0 && errno != EEXIST)
    {
        if (operation->debug)
            perror(tables_path);
    }

    for (size_t i = 0; i < static_tables_count; ++i)
    {
        char filename[4096];
        snprintf(filename, sizeof(filename), "%s/%s", tables_path, static_tables[i].name);

        FILE *f = fopen(filename, "w");
        if (f)
        {
            fclose(f); // Close immediately after creating an empty file
        }
        else if (operation->debug)
        {
            perror(filename); // Print error if debug is enabled
        }
    }
}
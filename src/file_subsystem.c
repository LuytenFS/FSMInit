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
    {"particle_effects", "tbl", true, "-part"} // special XMT-only case
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

void create_directories(const char *base_path)
{
    for (size_t i = 0; i < fs_dirs_count; ++i)
    {
        char dir_path[4096];
        snprintf(dir_path, sizeof(dir_path), "%s/%s", base_path, fs_dirs[i].name);

        if (mkdir(dir_path, 0755) != 0 && errno != EEXIST)
        {
            perror(dir_path);
            continue;
        }

        // subdirs
        for (size_t j = 0; j < fs_dirs[i].subdir_count; ++j)
        {
            char subdir_path[4096];
            snprintf(subdir_path, sizeof(subdir_path), "%s/%s", dir_path, fs_dirs[i].subdirs[j]);
            if (mkdir(subdir_path, 0755) != 0 && errno != EEXIST)
            {
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

    if (mkdir(tables_path, 0755) != 0 && errno != EEXIST)
    {
        if (operation->debug)
            perror(tables_path);
    }

    for (size_t i = 0; i < fs_tables_count; ++i)
    {
        const FS_TABLE_ENTRY *entry = &fs_tables[i];
        char filename[4096];

        const char *ext = (entry->is_modular && strcmp(operation->table_type, "-tbm") == 0) ? "tbm" : "tbl";

        if (strcmp(operation->table_type, "-tbm") == 0 && entry->is_modular && operation->prefix)
        {
            // Use the modular suffix
            snprintf(filename, sizeof(filename), "%s/%s%s.%s", tables_path, operation->prefix, entry->modular_suffix, ext);
        }
        else
        {
            snprintf(filename, sizeof(filename), "%s/%s.%s", tables_path, entry->base_name, ext);
        }

        FILE *f = fopen(filename, "w");
        if (f)
            fclose(f);
        else if (operation->debug)
            perror(filename);
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
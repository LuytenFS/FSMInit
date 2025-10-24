#pragma once

#include <stddef.h>
#include <stdbool.h>

/* =======================
   CLI Option Structure
   ======================= */
typedef struct OP
{
    const char *command;    // User command: "-stdm", "-stdmc", "-help"
    const char *path;       // Directory path for creating files/tables
    const char *table_type; // "-tbl" or "-tbm"
    const char *prefix;     // Optional prefix for .tbm (NULL if not used)
    int debug;              // 1 if -debug flag is set, 0 otherwise
} OP;

/* =======================
   Directory Definitions
   ======================= */
/*
cache/
cbanims/
config/
demos/
effects/
fonts/
force feedback/
freddocs/
hud/
intelanims/
interface/
maps/
missions/
models/
movies/
multidata/
music/
players/
images/
multi/
single/
squads/
scripts/
sounds/
8b22k/
16b11k/
tables/
text/
voice/ ~ expands into:
    /briefing/
    /command_briefings/
    /debriefing/
    /personas/
    /special/
    /training/
*/

typedef struct FS_DIRECTORIES
{
    const char *name;
    const char **subdirs;
    size_t subdir_count;
} FS_DIRECTORIES;

/* =======================
   Table Entry Definitions
   ======================= */
/*
ai.tbl can be extended with xxx-aic.tbm (XMT)
...
weapon_expl.tbl can be extended with xxx-wxp.tbm (non-XMT)
Particle Effects can be added with xxx-part.tbm (XMT; special type as it has no parent table)
*/

typedef struct FS_TABLE_ENTRY
{
    const char *base_name;      // e.g., "weapons"
    const char *extension;      // "tbl" or "tbm" — can be set dynamically at runtime
    bool is_modular;            // true if supports modular .tbm variants
    const char *modular_suffix; // e.g., "-aic", "-aip", "-amr"
} FS_TABLE_ENTRY;

/* =======================
   Static Table Definitions
   ======================= */
/*
Ai_profiles.tbl
Autopilot.tbl
Colors.tbl
Iff_defs.tbl
Objecttypes.tbl
Species_defs.tbl
Armor.tbl
Controlconfigdefaults.tbl
Scripting.tbl
*/

typedef struct FS_TABLES
{
    const char *name;
} FS_TABLES;

/* =======================
   External Declarations
   ======================= */
extern const char *voice_subdirs[];
extern const size_t voice_subdirs_count;

extern FS_DIRECTORIES fs_dirs[];
extern const size_t fs_dirs_count;

extern FS_TABLE_ENTRY fs_tables[];
extern const size_t fs_tables_count;

extern FS_TABLES static_tables[];
extern const size_t static_tables_count;

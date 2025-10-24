#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "def_type.h"

/*
Command-line arguments for the program:

1. <command>       : The operation to perform. Allowed values:
                     - "-stdm"   : Standard Mod (no tables)
                     - "-stdmc"  : Standard Mod Complex (with tables)
                     - "-help"   : Display usage information

2. <path>          : The directory path where the tool will create
                     directories and tables.

3. <-tbl/-tbm>     : Specifies whether to create standard .tbl files or
                     modular .tbm files.

4. <prefix>        : Optional prefix for .tbm files. Required only if
                     "-tbm" is specified. Ignored for "-tbl".

5. [-debug]        : Optional flag. Enables debug output to "log.txt" in
                     the program's current directory.
*/

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: <command> <path> <-tbl/-tbm> [prefix if -tbm] [optional -debug]\n");
        return 1;
    }

    OP operation = {0};
    operation.command = argv[1];
    operation.path = argv[2];
    operation.table_type = argv[3];
    operation.prefix = NULL;
    operation.debug = 0;

    // Handle optional prefix for .tbm
    if (strcmp(operation.table_type, "-tbm") == 0)
    {
        if (argc < 5)
        {
            fprintf(stderr, "Error: .tbm requires a prefix argument.\n");
            return 1;
        }
        operation.prefix = argv[4];
    }

    // Handle optional -debug flag
    for (int i = 4; i < argc; ++i)
    {
        if (strcmp(argv[i], "-debug") == 0)
        {
            operation.debug = 1;
            break;
        }
    }

    /*
       The 'operation' struct now contains all CLI input:
       - operation.command
       - operation.path
       - operation.table_type
       - operation.prefix
       - operation.debug
    */

    return 0;
}

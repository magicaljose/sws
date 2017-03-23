#ifndef COMMAND_H
#define COMMAND_H

#include "game.h"


#define CDB_MAX_ARG_COUNT  (2)


// Valid command statuses
typedef enum
{
    CS_OK           = 0,
    CS_BAD_CMD,
    CS_BAD_ARG      = 10,
    CS_BAD_ARG_1,
    CS_BAD_ARG_2,
    CS_MISSING_ARGS,
    CS_BAD_MOVE     = 20,
    CS_ERROR        = 50
} CmdError_t;

// Valid commands
typedef enum
{
    _INVALID_CMD,
    _CLEAR_CMD,
    _KEY_CMD,
    _MOVE_CMD,
    _FLIP_CMD,
    _QUIT_CMD,
    _UNDO_CMD,
    _REDO_CMD,
    _FORCE_CMD
} CmdId_t;

// Cdb pile item identifier
typedef struct _CdbPileItem_t
{
    PileType_t pileType;
    int id;
} CdbPileItem_t;

// Command descriptor block
typedef struct _Cdb_t
{
    CmdId_t cmdId;
    union
    {
        struct
        {
            CdbPileItem_t src;
            CdbPileItem_t dst;
        };
        CdbPileItem_t arg[CDB_MAX_ARG_COUNT];
    };
} Cdb_t;

#endif // COMMAND_H

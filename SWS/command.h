#ifndef COMMAND_H
#define COMMAND_H

#include <QMap>


// Valid command statuses
typedef enum
{
    GOOD,
    BAD_CMD,
    BAD_ARG_1,
    BAD_ARG_2,
    BAD_MOVE,
    MISSING_ARGS
} CmdStatus_t;

// Valid commands
typedef enum
{
    _CLEAR_CMD,
    _KEY_CMD,
    _MOVE_CMD,
    _FLIP_CMD,
    _QUIT_CMD,
    _UNDO_CMD,
    _REDO_CMD,
    _FORCE_CMD,
    _INVALID_CMD
} CmdId_t;

// CmdId_t map
#define CMD_MAP_PAIR_T QString, CmdId_t
typedef QMap<CMD_MAP_PAIR_T> CmdMap_t;

// Cdb stack item identifier
typedef struct _CdbStackItem_t
{
    int stackType;
    int id;
} CdbStackItem_t;

// Command descriptor block
typedef struct _Cdb_t
{
    CmdId_t cmdId;
    union
    {
        CdbStackItem_t src;
        CdbStackItem_t arg1;
    };
    union
    {
        CdbStackItem_t dst;
        CdbStackItem_t arg2;
    };
} Cdb_t;


void CommandParse(const QString &str, Cdb_t &newCdb);

#endif // COMMAND_H

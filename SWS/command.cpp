#include "command.h"


////////////////////////
// Standard functions

#define STRINGIFY(x)  #x
#define CMD_MAP_PAIR(m)  {STRINGIFY(m), _##m##_CMD}
#define CMD_MAP_PAIRS    \
    CMD_MAP_PAIR(CLEAR), \
    CMD_MAP_PAIR(KEY),   \
    CMD_MAP_PAIR(MOVE),  \
    CMD_MAP_PAIR(FLIP),  \
    CMD_MAP_PAIR(QUIT),  \
    CMD_MAP_PAIR(UNDO),  \
    CMD_MAP_PAIR(REDO),  \
    CMD_MAP_PAIR(FORCE)
// Parse command string
void CommandParse(const QString &str, Cdb_t &newCdb)
{
    static const CmdMap_t CmdMap = { CMD_MAP_PAIRS };
    QString s = str.toUpper();

    newCdb.cmdId = CmdMap.value(s, _INVALID_CMD);
}

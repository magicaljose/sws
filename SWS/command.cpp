#include <QMap>
#include "command.h"


typedef struct _CmdTableEntry_t
{
    bool arg1Needed;
    bool arg2Needed;
} CmdTableEntry_t;


const CmdTableEntry_t CmdTable[] =
{
    {false, false},
    {false, false},
    {true, true}
};


////////////////////////
// Standard functions



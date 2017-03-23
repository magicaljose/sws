#ifndef CONSOLE_H
#define CONSOLE_H

#include <QDebug>
#include "game.h"
#include "command.h"


// Table string control structure
typedef struct _ConsoleTable_t
{
    QString *pStr;
    int width;
} ConsoleTable_t;


const char * GetSuitStr(CardSuit_t suitId);
const char * GetSuitInitialStr(CardSuit_t suitId);
const char * GetCardStr(CardValue_t nameId);
const char * GetCardInitialStr(CardValue_t nameId);


// GameConsole class
class GameConsole
{
public:
    GameConsole();

    void printTable(PileMap_t &pileMap);

    CmdError_t collectInput(Cdb_t &cdb);
    CmdError_t collectInput(Cdb_t &cdb, QTextStream &is);

private:
    QTextStream & qOut();
    int calcTableWidth(PileMap_t &pileMap);
    int calcTableHeight(PileMap_t &pileMap);
    int imprintCard(Card *pCard, ConsoleTable_t &table, int strIdx, bool overlapBelow, bool overlapAbove);
    void imprintPile(Pile *pPile, ConsoleTable_t &table);
    CmdError_t tokenize(Cdb_t &cdb, const QStringList &wordList);
    CmdError_t getCmdId(const QString &str, CmdId_t &cmdId);
    CmdError_t getArg(const QString &str, CdbPileItem_t &pileItem);
};

#endif // CONSOLE_H

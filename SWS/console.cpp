#include <QMap>
#include <QDebug>
#include "console.h"
#include "command.h"
#include "game.h"

using namespace std;


#define STRINGIFY(x)  #x


// Array of printable card segments
const char *strTable[] =
{
    " ----- ",    // 0
    "|-----|",    // 1
    "|     |",    // 2
    "|\\ \\ \\|", // 3
    "| \\ \\ |"   // 4
};
#define CARD_HEIGHT                    (5)  // Number of printed lines for naked card
#define CARD_HEIGHT_OVERLAP            (3)  // Number of printed lines for overlapped card
#define CARD_HEIGHT_OVERLAP_FACE_DOWN  (1)  // Number of printed lines for overlapped, face-down
#define CARD_WIDTH                     (7)  // Does not include null terminator

// Matrix of card segments; line and card state used as indeces
const char *cardStrMatrix[][2] =
{
    {strTable[0], strTable[0]}, // Line 0
    {strTable[3], strTable[2]}, // Line 1
    {strTable[4], strTable[2]}, // Line 2
    {strTable[3], strTable[2]}, // Line 3
    {strTable[0], strTable[0]}  // Line 4
};
#define CARD_LINE(line, state)  (cardStrMatrix[line][state])
#define CARD_LINE_1_ALT         (strTable[1])


CmdError_t getCmdId(const QString &str, CmdId_t &cmdId);
CmdError_t getArg(const QString &str, CdbPileItem_t &pileItem);


////////////////////////////////
// GameConsole class methods

GameConsole::GameConsole()
{
}

// Standard output
#define qout  QTextStream(stdout)

// Standard input
inline QTextStream & qIn(FILE *is = stdin)
{
    static QTextStream s(is);
    return s;
}
#define qin  qIn()  // Assumes FILE ptr named 'is'
#define consume()  readLine()  // Consume remaining words


#define COL_WIDTH   (CARD_WIDTH + 1)
#define ROW_HEIGHT  (CARD_HEIGHT + 2)  // Include room for pile headers
#define TO_X_COORD(c)  ((c) * COL_WIDTH)
#define TO_Y_COORD(r)  ((r) * ROW_HEIGHT + 1)  // Offset by 1 for pile headers
// Return table width as determined by pile map configuration
int GameConsole::calcTableWidth(PileMap_t &pileMap)
{
    int x, y;
    int tableWidth = 0;

    // Search piles for largest x-coordinate
    for (auto pileVec : PILE_MAP)
    {
        for (auto pPile : pileVec)
        {
            pPile->getCoord(&x, &y);
            if (++x > tableWidth) tableWidth = x;
        }
    }

    return tableWidth * (CARD_WIDTH + 1);
}

// Return table height as determined by pile map configuration
int GameConsole::calcTableHeight(PileMap_t &pileMap)
{
    int x, y;
    int h;
    int cardCnt;
    QVector<int> heightV;
    int tableHeight = 0;

    // Search each level (y-coordinate) for largest pile height
    for (auto pileVec : PILE_MAP)
    {
        for (auto pPile : pileVec)
        {
            pPile->getCoord(&x, &y);
            if (y >= heightV.size()) heightV.push_back(0); // Check for new level
            switch (pPile->getPrintStyle())
            {
            case NOTHING:
                break;
            case CASCADE:
                cardCnt = pPile->getCardCount();
                if (cardCnt == 0) h = ROW_HEIGHT;
                else
                {
                    Card *pCard = pPile->bottomCard();
                    h = 0;
                    do
                    {
                        if (pPile->getCard(PREVIOUS) != nullptr)
                        {
                            // Reduce line count for overlap
                            h += (pCard->isFaceUp())? CARD_HEIGHT_OVERLAP : CARD_HEIGHT_OVERLAP_FACE_DOWN;
                        }
                        else h += ROW_HEIGHT;
                        pCard = pPile->prevCard();
                    } while(pCard != nullptr);
                }
                break;
            default:
                h = ROW_HEIGHT;
            }
            if (h > heightV[y]) heightV[y] = h;
        }
    }

    // Sum level heights
    for (auto val : heightV) tableHeight += val;

    return tableHeight;
}

#define TABLE_STR_COORD_IDX(x, y)       (((y) * table.width) + (x))
#define TABLE_STR_REPLACE(str, i, n)    (table.pStr->replace((i), n, str))
#define TABLE_CARD_STR_REPLACE(str, i)  TABLE_STR_REPLACE(str, i, CARD_WIDTH)
#define TABLE_IMPRINT(i, c, l)          TABLE_CARD_STR_REPLACE(CARD_LINE(l, ((c == nullptr)? true : c->isFaceUp())), i);
// Imprint single card to print string; return number of lines printed
int GameConsole::imprintCard(Card *pCard, ConsoleTable_t &table, int strIdx, bool overlapBelow, bool overlapAbove)
{
    int i = strIdx;
    int l;
    int lineCnt;

    // Imprint alternative first card line if necessary
    if (overlapBelow)
    {
        TABLE_CARD_STR_REPLACE(CARD_LINE_1_ALT, i);
        i += table.width;
        l = 1;
    }
    else l = 0;

    // Set number of lines to be printed based on card/pile state
    if (overlapAbove)
    {
        // Reduce 'lineCnt' for overlap
        lineCnt = (pCard->isFaceUp())? CARD_HEIGHT_OVERLAP : CARD_HEIGHT_OVERLAP_FACE_DOWN;
    }
    else lineCnt = CARD_HEIGHT;

    // Imprint remaining card lines
    for (; l < lineCnt; l++)
    {
        TABLE_IMPRINT(i, pCard, l);

        // Add card info for face-up cards
        if (pCard != nullptr && pCard->isFaceUp())
        {
            QString cardValue;

            switch (l)
            {
            case 1:
                cardValue = QString(GetCardInitialStr(pCard->getValue()));
                TABLE_STR_REPLACE(cardValue, i + 1, cardValue.size());
                break;
            case 2:
                TABLE_STR_REPLACE(GetSuitInitialStr(pCard->getSuit()), i + 3, 1);
                break;
            case 3:
                cardValue = QString(GetCardInitialStr(pCard->getValue()));
                TABLE_STR_REPLACE(cardValue, i + 6 - cardValue.size(), cardValue.size());
                break;
            }
        }

        i += table.width;
    }

    return lineCnt;
}

// Imprint pile
void GameConsole::imprintPile(Pile *pPile, ConsoleTable_t &table)
{
    int col, row;
    int x, y;
    Card *pCard;
    bool overlapBelow;
    bool overlapAbove;

    switch (pPile->getPrintStyle())
    {
    case CASCADE:
        pCard = pPile->bottomCard();
        pPile->getCoord(&col, &row);
        x = TO_X_COORD(col);
        y = TO_Y_COORD(row);
        do
        {
            overlapBelow = (pPile->getCard(NEXT) != nullptr);
            overlapAbove = (pPile->getCard(PREVIOUS) != nullptr);
            y += imprintCard(pCard, table, TABLE_STR_COORD_IDX(x, y), overlapBelow, overlapAbove);
            pCard = pPile->prevCard();
        } while (pCard != nullptr);
        break;

    case BOTTOM_CARD_ONLY:
        pPile->getCoord(&col, &row);
        x = TO_X_COORD(col);
        y = TO_Y_COORD(row);
        imprintCard(pPile->bottomCard(), table, TABLE_STR_COORD_IDX(x, y), false, false);
        break;

    case BOTTOM_CARD_NO_NULL:
        if (pPile->getCardCount() > 0)
        {
            pPile->getCoord(&col, &row);
            x = TO_X_COORD(col);
            y = TO_Y_COORD(row);
            imprintCard(pPile->bottomCard(), table, TABLE_STR_COORD_IDX(x, y), false, false);
        }
        break;

    case TOP_CARD_ONLY:
        pPile->getCoord(&col, &row);
        x = TO_X_COORD(col);
        y = TO_Y_COORD(row);
        imprintCard(pPile->topCard(), table, TABLE_STR_COORD_IDX(x, y), false, false);
        break;

    default:
        break;
    }
}

// Construct and print game table to console
void GameConsole::printTable(PileMap_t &pileMap)
{
    ConsoleTable_t table;
    int tableWidth = calcTableWidth(pileMap);
    int tableHeight = calcTableHeight(pileMap);

    // Sanity check dimensions
    if (tableWidth == 0 || tableHeight == 0) return;

    // Create table
    QString tableStr(tableHeight * tableWidth, ' ');
    table.pStr = &tableStr;
    table.width = tableWidth;
    for (auto line = 1; line <= tableHeight; line++)
    {
        tableStr[line * tableWidth - 1] = '\n';
    }

    // Imprint piles
    for (auto pileVec : PILE_MAP)
    {
        for (auto pPile : pileVec)
        {
            imprintPile(pPile, table);
        }
    }

    qout << tableStr;
}

// Collect console input
CmdError_t GameConsole::collectInput(Cdb_t &cdb)
{
    QString cmdStr;
    QStringList wordList;

    // Collect console input and parse
    cmdStr = qin.readLine();
    wordList = cmdStr.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    return tokenize(cdb, wordList); // Tokenize parsed input
}

// Fake console input
CmdError_t GameConsole::collectInput(Cdb_t &cdb, QTextStream &is)
{
    QString cmdStr;
    QStringList wordList;

    // Pull command string and parse
    cmdStr = is.readLine();
    wordList = cmdStr.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    return tokenize(cdb, wordList);
}

CmdError_t GameConsole::tokenize(Cdb_t &cdb, const QStringList &wordList)
{
    CmdError_t status = CS_ERROR;
    int a;

    // Collect command ID
    status = getCmdId(wordList[0], cdb.cmdId);

    // Collect CDB args; after one fails to tokenize, all remaining will
    //   be invalidated; excess words will be ignored
    for (a = 0; a < CDB_MAX_ARG_COUNT; a++)
    {
        if (a + 1 >= wordList.size() && status == CS_OK) status = CS_MISSING_ARGS;
        if (status == CS_OK)
        {
            // Collect input
            status = getArg(wordList[a + 1], cdb.arg[a]);
        }
        else
        {
            // Invalidate
            cdb.arg[a].pileType = INVALID_PILE_TYPE;
            cdb.arg[a].id       = INVALID_PILE_ID;
        }
    }
    if (a + 1 != wordList.size() && status == CS_OK) status = CS_TOO_MANY_ARGS;

    return status;
}

#define CMD_MAP_PAIR(c)  {STRINGIFY(c), _##c##_CMD}
#define CMD_MAP_PAIRS    \
    CMD_MAP_PAIR(CLEAR), \
    CMD_MAP_PAIR(KEY),   \
    {"HELP", _KEY_CMD},  \
    CMD_MAP_PAIR(MOVE),  \
    CMD_MAP_PAIR(FLIP),  \
    CMD_MAP_PAIR(QUIT),  \
    CMD_MAP_PAIR(UNDO),  \
    CMD_MAP_PAIR(REDO),  \
    CMD_MAP_PAIR(FORCE)
// Parse command string; generate command ID
CmdError_t GameConsole::getCmdId(const QString &str, CmdId_t &cmdId)
{
    static const QMap<QString, CmdId_t> CmdMap = { CMD_MAP_PAIRS };
    QString s = str.toUpper();

    cmdId = CmdMap.value(s, _INVALID_CMD);
    if (cmdId == _INVALID_CMD) return CS_BAD_CMD;

    return CS_OK;
}

// Parse command pile arg
#define PILE_TYPE_PAIRS \
    {'D', DECK},        \
    {'S', DISCARD},     \
    {'W', WASTE},       \
    {'F', FOUNDATION},  \
    {'C', CELL},        \
    {'T', TABLEAU}
CmdError_t GameConsole::getArg(const QString &str, CdbPileItem_t &pileItem)
{
    static const QMap<QChar, PileType_t> ArgPileMap = { PILE_TYPE_PAIRS };
    QString s = str.toUpper();
    bool ok;

    pileItem.pileType = ArgPileMap.value(s[0], INVALID_PILE_TYPE);
    s.remove(0, 1);
    pileItem.id = s.toInt(&ok, 10);
    if (!ok)
    {
        if (s.size() == 0)
        {
            // If no ID, default to 0
            pileItem.id = 0;
            ok = true;
        }
        else pileItem.id = INVALID_PILE_ID;
    }
    if (!ok || pileItem.pileType == INVALID_PILE_TYPE) return CS_BAD_ARG;

    return CS_OK;
}


////////////////////////////////
// Standard functions

// Get suit name
const char * GetSuitStr(CardSuit_t suitId)
{
    static const char * suitTable[] = {"Hearts", "Diamonds", "Clubs", "Spades"};

    return suitTable[suitId];
}

// Get suit initial
const char * GetSuitInitialStr(CardSuit_t suitId)
{
    static const char * suitITable[] = {"H", "D", "C", "S"};

    return suitITable[suitId];
}

// Get face name
const char * GetCardStr(CardValue_t nameId)
{
    static const char * nameTable[] =
    {
        "Joker",
        "Ace",
        "Two",
        "Three",
        "Four",
        "Five",
        "Six",
        "Seven",
        "Eight",
        "Nine",
        "Ten",
        "Jack",
        "Queen",
        "King"
    };

    return nameTable[nameId];
}

// Get face initial(s)
const char * GetCardInitialStr(CardValue_t nameId)
{
    static const char * cardITable[] =
        {"Jo", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

    return cardITable[nameId];
}

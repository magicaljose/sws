#ifndef GAME_H
#define GAME_H

#include <QMap>
#include <QVector>
#include <QCommandLineParser>
#include "game_common.h"
#include "console.h"
#include "command.h"


// Game pile class
class Pile
{
public:
    Pile(PileType_t pileType, int xLoc, int yLoc);

    inline int getCardCount()  { return pile.size(); }

    inline PilePrintStyle_t getPrintStyle()  { return printStyle; }

    inline void getCoord(int *pX, int *pY)  { *pX = loc.x; *pY = loc.y; }

    inline Card * getCard(int offset = 0)  { return pile.value(cardIt + offset, nullptr); }
    inline Card * nextCard()  { return pile.value(--cardIt, nullptr); }
    inline Card * prevCard()  { return pile.value(++cardIt, nullptr); }
    inline Card * topCard()     { cardIt = pile.size() - 1; return getCard(); }
    inline Card * bottomCard()  { cardIt = 0; return getCard(); }

    inline void push(Card *pNewCard)  { pile.push_back(pNewCard); }
    Card * pop();
    inline void pushToFront(Card *pNewCard)  { pile.push_front(pNewCard); }
    Card * popFromFront();

private:
    Pile_t pile;
    PileType_t type;
    PilePrintStyle_t printStyle;
    Coord_t loc;
    int cardIt;
};


// Standard game control class
class Game
{
public:
    Game(DeckType_t deckType,
         void (*checkForWinFunc)(PileMap_t &pileMap, GameState_t &state),
         CmdError_t (*validateCommandFunc)(Cdb_t &cdb),
         uint gameSeed = INVALID_SEED);

    inline bool isGameFinished()  { return (state == GAME_WON || state == GAME_OVER); }
    inline bool isGameWon()       { return (state == GAME_WON); }

    void registerPile(PileType_t pileType, int pileCount, int xLoc, int yLoc);

    GameError_t deal(PileType_t pileType = TABLEAU, DealMethod_t dealMethod = INCREMENTING);
    GameError_t moveCards(Pile *pSrcPile, Pile *pDstPile, int n);
    inline GameError_t moveCard(Pile *pSrcPile, Pile *pDstPile)  { return moveCards(pSrcPile, pDstPile, 1); }

    CmdError_t processCommand(Cdb_t &cdb);

    void print(GameConsole &console);

    inline uint getDeckSeed()  { return deckSeed; }

private:
    GameState_t state;
    PileMap_t pileMap;
    Deck deck;
    uint deckSeed;

    // Undefined functions
    void (*checkForWin)(PileMap_t &pileMap, GameState_t &state);
    CmdError_t (*validateCommand)(Cdb_t &cdb);
};


const QCommandLineOption & SetGameAppInfo(const QString &name, const QString &ver, const QString &description,
                    QCommandLineParser &parser);

#endif // GAME_H

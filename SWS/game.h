#ifndef GAME_H
#define GAME_H

#include <QMap>
#include <QVector>
#include "card.h"


#define PILE_MAP          (pileMap)
#define PILE_VECTOR(tid)  (pileMap[tid])
#define PILE(tid, pid)    (pileMap[tid][pid])
#define PILE_DECK         (pileMap[DECK][0])     // Assumes pile '0' as most
#define PILE_DISCARD      (pileMap[DISCARD][0])  //   games only use a single
#define PILE_WASTE        (pileMap[WASTE][0])    //   pile of this type

#define NEXT      (-1)  // Offset for next card
#define PREVIOUS  (1)   // Offset for previous card


class GameConsole;


// Pile types
typedef enum
{
    DECK,
    DISCARD,
    WASTE,
    FOUNDATION,
    CELL,
    TABLEAU,
    INVALID_PILE_TYPE
} PileType_t;

// Deck deal methods
typedef enum
{
    SINGLE,
    INCREMENTING,
    DECREMENTING,
    ALL
} DealMethod_t;

// Print styles (and standard uses)
/* DECK:        TOP_CARD_ONLY
 * DISCARD:     BOTTOM_CARD_NO_NULL
 * WASTE:       NOTHING
 * FOUNDATION:  BOTTOM_CARD_ONLY
 * CELL:        BOTTOM_CARD_ONLY
 * TABLEAU:     CASCADE */
typedef enum
{
    NOTHING,              // Nothing
    TOP_CARD_ONLY,        // Top card only
    BOTTOM_CARD_ONLY,     // Bottom card only
    BOTTOM_CARD_NO_NULL,  // Bottom card only, nothing on null
    CASCADE               // Cascade all cards in pile
} PilePrintStyle_t;

// Game method return statuses
typedef enum
{
    GS_OK                 = 0x00,  // OK
    GS_INS_PILE_SIZE      = 0x01,  // Insufficient pile size
    GS_EMPTY_PILE         = 0x02,  // Empty pile
    GS_ERROR              = 0x0f   // Misc error
} GameError_t;

// Game state
typedef enum
{
    GAME_IN_PROGRESS,  // Game may continue and is in progress
    GAME_ERROR,        // Game has encountered an error
    GAME_WON,          // Game has been won
    GAME_OVER          // Game has become unwinnable
} GameState_t;

// 2D coordinate container
typedef struct _Coord_t
{
    int x;
    int y;
} Coord_t;

typedef QVector<Card *> Pile_t;                    // A vector of [pointers to] cards that form a pile
typedef QVector<class Pile *> PileVector_t;        // Vector of [pointers to] pile objects of a particular type
typedef QMap<PileType_t, PileVector_t> PileMap_t;  // Mapping of all piles/types on table


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
         uint gameSeed = INVALID_SEED);

    inline bool isGameFinished()  { return (state == GAME_WON || state == GAME_OVER); }
    inline bool isGameWon()       { return (state == GAME_WON); }

    void registerPile(PileType_t pileType, int pileCount, int xLoc, int yLoc);

    GameError_t deal(PileType_t pileType = TABLEAU, DealMethod_t dealMethod = INCREMENTING);
    GameError_t moveCards(Pile *pSrcPile, Pile *pDstPile, int n);
    inline GameError_t moveCard(Pile *pSrcPile, Pile *pDstPile)  { return moveCards(pSrcPile, pDstPile, 1); }

    void print(GameConsole &console);

    inline uint getDeckSeed()  { return deckSeed; }

private:
    GameState_t state;
    PileMap_t pileMap;
    Deck deck;
    uint deckSeed;

    // Undefined functions
    void (*checkForWin)(PileMap_t &pileMap, GameState_t &state);
};

#endif // GAME_H

#ifndef GAME_COMMON_H
#define GAME_COMMON_H

#include "card.h"


#define PILE_MAP          (pileMap)
#define PILE_VECTOR(tid)  (pileMap[tid])
#define PILE(tid, pid)    (pileMap[tid][pid])
#define PILE_DECK         (pileMap[DECK][0])     // Assumes pile '0' as most
#define PILE_DISCARD      (pileMap[DISCARD][0])  //   games only use a single
#define PILE_WASTE        (pileMap[WASTE][0])    //   pile of this type

#define NEXT      (-1)  // Offset for next card
#define PREVIOUS  (1)   // Offset for previous card

#define INVALID_PILE_ID  (-1)


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
#define IS_VALID_PILE_TYPE(p)  ((p) != INVALID_PILE_TYPE)

typedef QVector<Card *> Pile_t;                    // A vector of [pointers to] cards that form a pile
typedef QVector<class Pile *> PileVector_t;        // Vector of [pointers to] pile objects of a particular type
typedef QMap<PileType_t, PileVector_t> PileMap_t;  // Mapping of all piles/types on table


class Pile;
class Game;

#endif // GAME_COMMON_H

#ifndef CARD_H
#define CARD_H

#include <QVector>


#define SUITS_PER_STD_DECK  (4)
#define CARDS_PER_STD_SUIT  (13)
#define CARDS_PER_STD_DECK  (SUITS_PER_STD_DECK * CARDS_PER_STD_SUIT)

#define INVALID_SEED  (0)


// Suit values
typedef enum
{
    HEARTS   = 0,
    DIAMONDS = 1,
    CLUBS    = 2,
    SPADES   = 3
} CardSuit_t;

// Face values
typedef enum
{
    JOKER   = 0,
    ACE     = 1,
    TWO     = 2,
    THREE   = 3,
    FOUR    = 4,
    FIVE    = 5,
    SIX     = 6,
    SEVEN   = 7,
    EIGHT   = 8,
    NINE    = 9,
    TEN     = 10,
    JACK    = 11,
    QUEEN   = 12,
    KING    = 13
} CardValue_t;

// Face states
typedef enum
{
    FACE_DOWN,
    FACE_UP
} CardState_t;

// Deck types
typedef enum
{
    ONE_SUIT_DECK   = 1,
    TWO_SUIT_DECK   = 2,
    THREE_SUIT_DECK = 3,
    STD_DECK        = 4
} DeckType_t;


class Deck
{
public:
    Deck(DeckType_t deckType = STD_DECK);

    QVector<class Card *> & getCardList()  { return cardList; }

    unsigned shuffle(unsigned seed = INVALID_SEED);

private:
    QVector<class Card *> cardList;
};


class Card
{
public:
    Card(CardSuit_t cardSuit    = SPADES,
         CardValue_t cardValue  = ACE,
         CardState_t cardState  = FACE_DOWN);

    inline CardSuit_t getSuit()  { return suit; }
    inline bool isRed()          { return (suit < CLUBS); }

    inline CardValue_t getValue()  { return value; }

    inline bool isFaceUp()      { return faceUp; }
    inline void flipFaceUp()    { faceUp = true; }
    inline void flipFaceDown()  { faceUp = false; }

private:
    CardSuit_t suit;
    CardValue_t value;
    bool faceUp;
};

#endif // CARD_H

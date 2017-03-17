#include <random>
#include <chrono>
#include <QDebug>
#include "card.h"

using namespace std;


////////////////////////
// Deck class methods

// Init Deck object
Deck::Deck(DeckType_t deckType)
{
    int suitMax = deckType;
    int suitId;
    int cardId;

    // Create cards and append to list
    for (suitId = HEARTS; suitId < suitMax; suitId++)
    {
        for (cardId = ACE; cardId <= KING; cardId++)
        {
            cardList.append(new Card((CardSuit_t)suitId, (CardValue_t)cardId));
        }
    }
}

// Shuffle deck and return seed
unsigned Deck::shuffle(unsigned seed)
{
    if (seed == INVALID_SEED)
    {
        seed = chrono::system_clock::now().time_since_epoch().count();
    }
    std::shuffle(cardList.begin(), cardList.end(), default_random_engine(seed));

    return seed;
}


////////////////////////
// Card class methods

// Init Card object
Card::Card(CardSuit_t cardSuit, CardValue_t cardValue, CardState_t cardState)
{
    suit = cardSuit;
    value = cardValue;

    faceUp = (cardState == FACE_UP)? true : false;
}

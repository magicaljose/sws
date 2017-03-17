#include <QDebug>
#include "game.h"
#include "card.h"
#include "console.h"

using namespace std;


////////////////////////
// Pile class methods

Pile::Pile(PileType_t pileType, int xLoc, int yLoc)
{
    type = pileType;

    // Set style options based on pile type
    switch (pileType)
    {
    case DECK:
        printStyle = TOP_CARD_ONLY;
        break;
    case DISCARD:
        printStyle = BOTTOM_CARD_NO_NULL;
        break;
    case FOUNDATION:
    case CELL:
        printStyle = BOTTOM_CARD_ONLY;
        break;
    case TABLEAU:
        printStyle = CASCADE;
        break;
    case WASTE:
    default:
        printStyle = NOTHING;
    }

    // Set location
    loc.x = xLoc;
    loc.y = yLoc;

    cardIt = 0;
}

// Pop Card ptr from back of vector; if empty return 'nullptr'
Card * Pile::pop()
{
    Card *pCard;

    if (!pile.empty())
    {
        pCard = pile.last();
        pile.pop_back();
    }
    else pCard = nullptr;

    return pCard;
}

// Pop Card ptr from front of vector; if empty return 'nullptr'
Card * Pile::popFromFront()
{
    Card *pCard;

    if (!pile.empty())
    {
        pCard = pile.first();
        pile.pop_front();
    }
    else pCard = nullptr;

    return pCard;
}


///////////////////
// Game class methods

// Init Game object
Game::Game(DeckType_t deckType,
           void (*checkForWinFunc)(PileMap_t &pileMap, GameState_t &state),
           uint gameSeed) : deck(deckType)
{
    state = GAME_IN_PROGRESS;

    checkForWin = checkForWinFunc;

    // Deck will have been instantiated; shuffle here and record seed
    deckSeed = deck.shuffle(gameSeed);
}

// Register pile with game
void Game::registerPile(PileType_t pileType, int pileCount, int xLoc, int yLoc)
{
    int x = xLoc;
    int y = yLoc;
    bool newPileVec = (PILE_MAP.contains(pileType) == false);

    // Add pile set vector if not already created
    if (newPileVec)
    {
        PILE_MAP.insert(pileType, {});
    }

    // Create empty pile vectors
    for (auto p = 0; p < pileCount; p++)
    {
        Pile *pNewPile = new Pile(pileType, x++, y);
        PILE_VECTOR(pileType).insert(PILE_VECTOR(pileType).end(), pNewPile);
    }

    // If registering DECK, init with cards
    if ((pileType == DECK) && newPileVec)
    {
        for (auto pCard : deck.getCardList())
        {
            PILE_DECK->push(pCard);
        }
    }
}

// Deal cards to piles as specified; returns 'true' if no cards to deal
bool Game::deal(PileType_t pileType, DealMethod_t dealMethod)
{
    if (PILE_DECK->getCardCount() == 0) return true;

    switch (dealMethod)
    {
    case SINGLE:
        for (auto pPile : PILE_VECTOR(pileType))
        {
            moveCard(PILE_DECK, pPile);
            pPile->topCard()->flipFaceUp();
        }
        break;

    case INCREMENTING:
        for (auto i = 0; i < PILE_VECTOR(pileType).size(); i++)
        {
            for (auto j = i; j < PILE_VECTOR(pileType).size(); j++)
            {
                moveCard(PILE_DECK, PILE(pileType, j));
                if (i == j) PILE(pileType, j)->topCard()->flipFaceUp();
            }
        }
        break;

    case DECREMENTING:
        for (auto i = PILE_VECTOR(pileType).size(); i >= 0; i--)
        {
            for (auto j = 0; j < i; j++)
            {
                moveCard(PILE_DECK, PILE(pileType, j));
                if (j == (i - 1)) PILE(pileType, j)->topCard()->flipFaceUp();
            }
        }
        break;

    case ALL:
        while (PILE_DECK->getCardCount() != 0)
        {
            for (auto pPile : PILE_VECTOR(pileType))
            {
                moveCard(PILE_DECK, pPile);
            }
        }
        for (auto pPile : PILE_VECTOR(pileType))
        {
            pPile->topCard()->flipFaceUp();
        }
        break;
    }

    return false;
}

// Move card(s) from one pile to another
GameError_t Game::moveCards(Pile *pSrcPile, Pile *pDstPile, int n)
{
    Card **pCard;

    // Check card count of source pile
    if (pSrcPile->getCardCount() < n) return GS_EMPTY_PILE;

    pCard = new Card * [n];

    // Pull from srcPile
    for (auto i = 0; i < n; i++)
    {
        pCard[i] = pSrcPile->popFromFront();
    }

    // Push to dstPile
    for (auto i = 0; i < n; i++)
    {
        pDstPile->push(pCard[i]);
    }

    return GS_OK;
}

// Print game table
void Game::print(GameConsole &console)
{
    console.printTable(pileMap);
}

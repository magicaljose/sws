#include <QString>
#include <QtTest>

#define private public
#include "../SWS/game.h"


void stub_checkForWin(PileMap_t &pileMap, GameState_t &state);


// Test class for SWS project
class SWS_Test : public QObject
{
    Q_OBJECT

public:
    SWS_Test();

private Q_SLOTS:
    void testBasicGameInit();
    void testPileRegistration();
    void testCardXfer();
};


////////////////////////////
// SWS_Test class methods

SWS_Test::SWS_Test()
{
}

// Test basic game object init
void SWS_Test::testBasicGameInit()
{
    Game testGame(STD_DECK, stub_checkForWin);

    // Verify card count
    QVERIFY(testGame.deck.cardList.size() == CARDS_PER_STD_DECK);
}

// Test game pile registration
void SWS_Test::testPileRegistration()
{
    Game testGame(STD_DECK, stub_checkForWin);

    // Verify empty pile map
    QVERIFY(testGame.pileMap.size() == 0);

    // Register DISCARD pile vector and confirm no cards assigned
    testGame.registerPile(DISCARD, 1, 1, 0);
    QVERIFY(testGame.pileMap.size() == 1);
    QVERIFY(testGame.pileMap[DISCARD].size() == 1);
    QVERIFY(testGame.pileMap[DISCARD][0]->pile.size() == 0);

    // Register DECK and confirm cards assigned
    testGame.registerPile(DECK, 1, 0, 0);
    QVERIFY(testGame.pileMap.size() == 2);
    QVERIFY(testGame.pileMap[DECK].size() == 1);
    QVERIFY(testGame.pileMap[DECK][0]->pile.size() == CARDS_PER_STD_DECK);

    // Register 2 FOUNDATION, 3 CELL, 9 TABLEAU piles
    testGame.registerPile(FOUNDATION, 2, 3, 0);
    testGame.registerPile(CELL, 3, 6, 0);
    testGame.registerPile(TABLEAU, 9, 0, 1);
    QVERIFY(testGame.pileMap.size() == 5); // Vector count
    QVERIFY(testGame.pileMap[FOUNDATION].size() == 2);
    QVERIFY(testGame.pileMap[CELL].size() == 3);
    QVERIFY(testGame.pileMap[TABLEAU].size() == 9);
}

// Test card transfers between piles
void SWS_Test::testCardXfer()
{
    Game testGame(STD_DECK, stub_checkForWin);
    GameError_t status;

    // Register piles
    testGame.registerPile(DECK, 1, 0, 0);
    testGame.registerPile(TABLEAU, 4, 0, 1);

    // Move 1 card to TABLEAU pile 0 and 2 cards to TABLEAU pile 2
    status = testGame.moveCard(testGame.pileMap[DECK][0], testGame.pileMap[TABLEAU][0]);
    QVERIFY(status == GS_OK);
    status = testGame.moveCards(testGame.pileMap[DECK][0], testGame.pileMap[TABLEAU][2], 2);
    QVERIFY(status == GS_OK);
    QVERIFY(testGame.pileMap[TABLEAU][0]->getCardCount() == 1);
    QVERIFY(testGame.pileMap[TABLEAU][1]->getCardCount() == 0);
    QVERIFY(testGame.pileMap[TABLEAU][2]->getCardCount() == 2);
    QVERIFY(testGame.pileMap[TABLEAU][3]->getCardCount() == 0);
    QVERIFY(testGame.pileMap[DECK][0]->getCardCount() == 49);

    // Attempt to move from empty pile
    status = testGame.moveCard(testGame.pileMap[TABLEAU][1], testGame.pileMap[TABLEAU][3]);
    QVERIFY(status == GS_EMPTY_PILE);
    QVERIFY(testGame.pileMap[TABLEAU][1]->getCardCount() == 0);
    QVERIFY(testGame.pileMap[TABLEAU][3]->getCardCount() == 0);
}


////////////////////////
// Standard functions

// Check for win stub
void stub_checkForWin(PileMap_t &, GameState_t &)
{
}


QTEST_APPLESS_MAIN(SWS_Test)

#include "tst_sws_test.moc"

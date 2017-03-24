#include <QString>
#include <QtTest>

#define private public
#include "../SWS/game.h"
#include "../SWS/console.h"


#define TEST_INPUT(s)  QTextStream(s)


void stub_checkForWin(PileMap_t &pileMap, GameState_t &state);


// Test class for SWS project
class SWS_Test : public QObject
{
    Q_OBJECT

public:
    SWS_Test();

private Q_SLOTS:
    // Game control tests
    void testBasicGameInit();
    void testPileRegistration();
    void testCardXfer();

    // Console tests
    void testConsoleInputParsing();
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

// Test command line <-> CDB parsing
void SWS_Test::testConsoleInputParsing()
{
    GameConsole console;
    Cdb_t testCdb;
    CmdError_t status;

    // Parse command with 2 good args
    status = console.collectInput(testCdb, TEST_INPUT("move t1 f3"));
    QVERIFY(status == CS_OK); // Max args set to '2' so should return 'CS_OK'
    QVERIFY(testCdb.cmdId == _MOVE_CMD);
    QVERIFY(testCdb.arg[0].pileType == TABLEAU);
    QVERIFY(testCdb.arg[0].id == 1);
    QVERIFY(testCdb.arg[1].pileType == FOUNDATION);
    QVERIFY(testCdb.arg[1].id == 3);

    // Parse command with 1 good arg (and no pile ID)
    status = console.collectInput(testCdb, TEST_INPUT("FLIP D"));
    QVERIFY(status == CS_MISSING_ARGS); // No arg 2
    QVERIFY(testCdb.cmdId == _FLIP_CMD);
    QVERIFY(testCdb.arg[0].pileType == DECK);
    QVERIFY(testCdb.arg[0].id == 0);
    QVERIFY(testCdb.arg[1].pileType == INVALID_PILE_TYPE);
    QVERIFY(testCdb.arg[1].id == INVALID_PILE_ID);

    // Parse command with no args
    status = console.collectInput(testCdb, TEST_INPUT("key"));
    QVERIFY(status == CS_MISSING_ARGS);
    QVERIFY(testCdb.cmdId == _KEY_CMD);
    QVERIFY(testCdb.arg[0].pileType == INVALID_PILE_TYPE);
    QVERIFY(testCdb.arg[0].id == INVALID_PILE_ID);
    QVERIFY(testCdb.arg[1].pileType == INVALID_PILE_TYPE);
    QVERIFY(testCdb.arg[1].id == INVALID_PILE_ID);

    // Parse command with bad command string
    status = console.collectInput(testCdb, TEST_INPUT("floop f2"));
    QVERIFY(status == CS_BAD_CMD);
    QVERIFY(testCdb.cmdId == _INVALID_CMD);
    QVERIFY(testCdb.arg[0].pileType == INVALID_PILE_TYPE);
    QVERIFY(testCdb.arg[0].id == INVALID_PILE_ID);
    QVERIFY(testCdb.arg[1].pileType == INVALID_PILE_TYPE);
    QVERIFY(testCdb.arg[1].id == INVALID_PILE_ID);

    // Parse 'help' command with bad arg (invalid pile ID)
    status = console.collectInput(testCdb, TEST_INPUT("help cd"));
    QVERIFY(status == CS_BAD_ARG);
    QVERIFY(testCdb.cmdId == _KEY_CMD);
    QVERIFY(testCdb.arg[0].pileType == CELL);
    QVERIFY(testCdb.arg[0].id == INVALID_PILE_ID);
    QVERIFY(testCdb.arg[1].pileType == INVALID_PILE_TYPE);
    QVERIFY(testCdb.arg[1].id == INVALID_PILE_ID);

    // Parse command with bad arg (invalid pile type)
    status = console.collectInput(testCdb, TEST_INPUT("force move f4"));
    QVERIFY(status == CS_BAD_ARG);
    QVERIFY(testCdb.arg[0].pileType == INVALID_PILE_TYPE);
    QVERIFY(testCdb.arg[0].id == INVALID_PILE_ID);
    QVERIFY(testCdb.arg[1].pileType == INVALID_PILE_TYPE);
    QVERIFY(testCdb.arg[1].id == INVALID_PILE_ID);

    // Parse command with excess args
    status = console.collectInput(testCdb, TEST_INPUT("undo f0 t1 c3"));
    QVERIFY(status == CS_TOO_MANY_ARGS);
    QVERIFY(testCdb.arg[0].pileType == FOUNDATION);
    QVERIFY(testCdb.arg[0].id == 0);
    QVERIFY(testCdb.arg[1].pileType == TABLEAU);
    QVERIFY(testCdb.arg[1].id == 1);
    QVERIFY(testCdb.src.pileType == testCdb.arg[0].pileType); // Quick alignment test
    QVERIFY(testCdb.src.id == testCdb.arg[0].id);             //
    QVERIFY(testCdb.dst.pileType == testCdb.arg[1].pileType); //
    QVERIFY(testCdb.dst.id == testCdb.arg[1].id);             //
}


////////////////////////
// Standard functions

// Check for win stub
void stub_checkForWin(PileMap_t &, GameState_t &)
{
}


QTEST_APPLESS_MAIN(SWS_Test)

#include "tst_sws_test.moc"

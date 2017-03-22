#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "console.h"
#include "game.h"
#include "card.h"
#include "command.h"

using namespace std;


#define KLONDIKE_TABLEAU_COUNT     (7)
#define KLONDIKE_FOUNDATION_COUNT  (4)


// Check for winning condition
void klondikeCheckForWin(PileMap_t &pileMap, GameState_t &state)
{
    // Cycle foundations
    for (auto pPile : PILE_VECTOR(FOUNDATION))
    {
        if (pPile->topCard()->getValue() != KING)
        {
            return;
        }
    }
    state = GAME_WON;
}

// Klondike game loop
int Klondike(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    bool gameSeedOk;
    uint gameSeed = 0;
    Cdb_t cdb;
    CmdError_t cmdStatus;

    // Set up parser
    QCoreApplication::setApplicationName("Klondike");
    QCoreApplication::setApplicationVersion("2.0");
    parser.setApplicationDescription("SWS Klondike console game");
    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption verOption = parser.addVersionOption();
    const QCommandLineOption seedOption(QStringList() << "s" << "seed",
        QCoreApplication::translate("main", "Set game seed."),
        QCoreApplication::translate("main", "seed"));
    parser.addOption(seedOption);

    // Parse and handle
    parser.process(app);
    if (parser.isSet(seedOption))
    {
        gameSeed = parser.value(seedOption).toUInt(&gameSeedOk);
        if (!gameSeedOk) gameSeed = 0; // Reset if failed
    }

    // Create game control object
    Game klondike(STD_DECK, klondikeCheckForWin, gameSeed);
    GameConsole console;
    qDebug() << "... Game object instantiated";
    qDebug() << "... Game seed:" << klondike.getDeckSeed();

    // Init game piles
    klondike.registerPile(DECK, 1, 0, 0);
    klondike.registerPile(DISCARD, 1, 1, 0);
    klondike.registerPile(FOUNDATION, KLONDIKE_FOUNDATION_COUNT, 3, 0);
    klondike.registerPile(TABLEAU, KLONDIKE_TABLEAU_COUNT, 0, 1);
    qDebug() << "... Piles registered";

    // Deal cards to piles
    klondike.deal(TABLEAU, INCREMENTING);
    qDebug() << "... Cards dealt";

    // Game loop
    do
    {
        klondike.print(console); // Print table
        cmdStatus = console.collectInput(cdb); // Collect input
        // Handle command
        break;
    } while(!klondike.isGameFinished());
    qDebug() << "... Game loop exited";

    return 0;
}

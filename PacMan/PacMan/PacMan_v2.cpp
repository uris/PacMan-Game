#include "Character.h"
#include "Player.h"
#include "Ghost.h"
#include "Game.h"
#include "Level.h"

using namespace std;
using namespace std::chrono;


int main()
{
    // create game objects
    Game game;
    Level level;
    Player player;
    Ghost redGhost(Ghosts::RED);
    Ghost yellowGhost(Ghosts::YELLOW);
    Ghost blueGhost(Ghosts::BLUE);
    Ghost pinkGhost(Ghosts::PINK);

    // add objects to game
    game.Add(level);
    game.Add(player);
    game.Add(redGhost, yellowGhost, blueGhost, pinkGhost);

    // start and run the game
    game.RunGame();
    
    // clean exit
    return 0;
}
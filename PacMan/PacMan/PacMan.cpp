#include "Player.h"
#include "Ghost.h"
#include "Game.h"
#include "Level.h"


int main()
{
    // create objects
    Game game;
    Level level;
    Player player;
    Ghost red(Ghosts::RED), yellow(Ghosts::YELLOW), blue(Ghosts::BLUE), pink(Ghosts::PINK);

    // add level, player and the four ghost objects to the game object
    game.Add(level);
    game.Add(player);
    game.Add(red, yellow, blue, pink);

    // run the game
    game.RunGame();
    
    // clean exit
    return 0;
}
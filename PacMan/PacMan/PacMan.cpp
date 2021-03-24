#include "Player.h"
#include "Ghost.h"
#include "Game.h"
#include "Level.h"


int main()
{
    // create objects
    Game game;
    Level* level = new Level;
    Player* player = new Player;
    Ghost* red = new Ghost(Ghosts::RED);
    Ghost* yellow = new Ghost(Ghosts::YELLOW);
    Ghost* blue = new Ghost(Ghosts::BLUE);
    Ghost* pink = new Ghost(Ghosts::PINK);

    // add level, player and the four ghosts to the game
    game.Add(level);
    game.Add(player);
    game.Add(red, yellow, blue, pink);

    // run the game
    game.RunGame();
    
    // clean exit
    return 0;
}
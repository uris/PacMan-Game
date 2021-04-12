#include "Game.h"
#include "Editor.h"
#include "Draw.h"
#include "MainMenu.h"

int main()
{
    //Draw::ShowColors(500);
    do
    {
        // play opening credits and choose edit/play
        string option = Draw::Credits();
        
        if (option == "#play") // play game
        {
            system("cls");
            Game game;
            game.RunGame();
        }
        else if (option == "#edit") // edit levels
        {
            system("cls");
            Editor editor;
            editor.EditScenes();
        }
        else if (option == "#res") // set display resolution
        {
            continue;
        }
        else // quit
        {
            break;
        }

    } while (true);
    

    //clean exit
    return 0;
}
#include "Game.h"
#include "Editor.h"
#include "Draw.h"
#include "MainMenu.h"

int main()
{
   
    do
    {
        // play opening credits and choose edit/play
        string option = Draw::Credits();

        // run edit, play or quit based on selection
        if (option == "#play")
        {
            system("cls");
            Game game;
            game.RunGame();
        }
        else if (option == "#edit")
        {
            system("cls");
            Editor editor;
            editor.EditScenes();
        }
        else
        {
            break;
        }

    } while (true);
    

    //clean exit
    return 0;
}
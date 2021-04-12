#include "Game.h"
#include "Editor.h"

int main()
{
    do
    {
        // play opening credits and choose edit/play/res
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
        else if (option == "#res") // display resolution was set
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
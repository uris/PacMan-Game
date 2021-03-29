#include "Editor.h"
#include "Utility.h"
#include "Draw.h"
#include <iostream>
#include <conio.h>
#include <thread>
#include <chrono>

using namespace std;

//constructor
Editor::Editor()
{
    p_cursor = new Cursor();
    p_scene = new Scene();
    p_cursor->SetEditor(this);
    p_scene->SetEditor(this);
};

// destructor
Editor::~Editor()
{
    if (p_cursor)
    {
        delete p_cursor;
        p_cursor = nullptr;
    }
    if (p_scene)
    {
        delete p_scene;
        p_scene = nullptr;
    }
    
};

// flow control
void Editor::EditScenes()
{
    MainMenu menu;
    MainMenu options;

    do
    {
        // reset editing state
        done_editing = false;
        exit_editing = false;
        is_saved = false;
        cancel_edit = false;

        // create menu of scenes to edit, display it and load selected scene.
        menu.Create();
        menu.Template(MenuTemplates::LIST_SCENES);
        string selected_scene = menu.Show();
        int load_scene = stoi(selected_scene);

        // do the editing of the selcted scene
        DoSceneEdit(load_scene);

        // check cancel - restart edit loop if cancel is true
        if (cancel_edit) {
            system("cls");
            continue;
        }

        // cls
        system("cls");

        // process the selected option, setting the value or saving the scene edits
        string selected_option = "#none";

        while (selected_option != "#save")
        {
            // show the menu of options
            options.Create(p_scene->GetSceneValues(), 11);
            options.Template(MenuTemplates::EDIT_SCENE_OPTIONS);
            string selected_option = options.Show();

            // process the selected option
            if (selected_option == "#save")
            {
                // do save stuff
                is_saved = true;
            }
            else
            {
                // update the scene option values
                UpdateValue(selected_option);
            }
           
            // clear
            system("cls");
        }

    } while (!is_saved);

    system("cls");
    cout << "save successful";
    

}

void Editor::GetKeyboardInput()
{
    int input;

    do
    {
        input = _getch();

        //if special character then get special key into input
        (input && input == 224) ? input = _getch() : input;

        switch (input)
        {
        case Globals::kARROW_LEFT:
            p_cursor->SetDirection(Direction::LEFT);
            break;
        case Globals::kARROW_RIGHT:
            p_cursor->SetDirection(Direction::RIGHT);
            break;
        case Globals::kARROW_UP:
            p_cursor->SetDirection(Direction::UP);
            break;
        case Globals::kARROW_DOWN:
            p_cursor->SetDirection(Direction::DOWN);
            break;
        case Globals::kSPACE:
            if (p_cursor->IsEditing()) {
                if (p_cursor->Pen() == '0')
                {
                    p_cursor->SetPen(' ');
                }
                else
                {
                    p_cursor->ChangeMode(false);
                }
            }
            else
            {
                p_cursor->ChangeMode(false);
            }
            break;
        case 'y':
            if (done_editing) {
                cancel_edit = false;
                done_editing = true;
                exit_editing = true;
            }
            break;
        case 'n':
            if (done_editing) {
                cancel_edit = true;
                done_editing = true;
                exit_editing = true;
                break;
            }
        case Globals::kESCAPE:
            if (p_cursor->IsEditing()) {
                if (p_cursor->Pen() != '0')
                {
                    p_cursor->SetPen('0');
                }
                else
                {
                    p_cursor->ChangeMode();
                }
            }
            else if (done_editing)
            {
                done_editing = false;
            }
            else
            {
                done_editing = true;
            }
            break;
        case '#':
        case '%':
        case '^':
        case '.':
        case 'o':
        case 't':
        case 's':
            if (p_cursor->IsEditing()) {
                p_cursor->SetPen(char(input));
            }
            break;
        default:
            break;
        }
    } while (!exit_editing);
}

void Editor::SetRefreshDelay()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(Globals::editor_refresh_delay)); // refresh rate using pause before next cycle
}

bool Editor::IsDoneEditing()
{
    return done_editing;
}

void Editor::SetDoneEditing(bool state)
{
    done_editing = state;
}

void Editor::UpdateValue(string option)
{
    system("cls"); Draw::ShowConsoleCursor(true);
    int int_input = 0;
    string string_input = "";
    option = Utility::TransformString(option, 1);

    if (option == "0") //scen #
    {
        cout << "Enter the new scene number: ";
        cin >> int_input;
        p_scene->this_scene = int_input;
    }

    if (option == "1") // title
    {
        cout << "Enter the new title: ";
        getline(cin, string_input);
        p_scene->title = string_input;
    }

    if (option == "2")
    {

    }

    if (option == "3")
    {

    }

    if (option == "4")
    {

    }

    if (option == "5")
    {

    }

    if (option == "6")
    {

    }

    if (option == "7")
    {

    }

    if (option == "8")
    {

    }

    if (option == "9")
    {

    }

    if (option == "0")
    {

    }
}

void Editor::DoSceneEdit(int load_scene)
{
    // clear screen
    system("cls");

    // Create the scene based on the selection
    p_scene->CreateLevelScene(load_scene);
    p_scene->DrawLevel();

    // Get keyboard entry for editing scene
    thread inputThread(&Editor::GetKeyboardInput, this); // new the ref() wrapper to pass by ref into thread

    // Edit the scene while not done editing
    do
    {
        // move the cursor
        p_cursor->MoveCursor();

        //draw the edited scene
        p_scene->DrawLevel();

        // refresh rate to slow down render
        SetRefreshDelay();


    } while (!exit_editing);

    // join the keyboard thread back to the main thread
    inputThread.join();
}

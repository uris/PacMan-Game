#include <fstream> // file manipulation
#include <iostream> // print to console
#include <stdlib.h> // exit
#include <conio.h> // used to get input without requiring return
#include "Scene.h"
#include "Utility.h"
#include "Draw.h"
#include "Editor.h"
#include <chrono>
#include <thread>

using namespace std;

//constructors
Scene::Scene()
{
	// do nothing for now
}

//destructors
Scene::~Scene()
{
    // deallcate options array
    DeleteOptions();
    
    // deallocated map array
    DeallocateMapArray();

    // nullify editor pointer
    if (p_editor)
    {
        p_editor = nullptr;
    }
}

// Public Methods
void Scene::CreateLevelScene(int& current_scene)
{
    // load level info and map from scenes file using games current level
    string map = LoadSceneFromFile("PacMan.scenes", current_scene);

    // if the string return "false" then you've reached the end of the game
    if (map == "false") {
        system("cls");
        cout << "You beat the PC! Good for you" << endl;
        system("pause");
        exit(0);
    }

    // parse through string to replace pellt and powerup markers to their ascii code
    Utility::ReplaceString(map, ".", char(Globals::pellet));

    // create dynamic two dimension pointer array to hold map
    Coord size = MapSize(map); // get width and height of the map
    cols = size.col; // set lsizes in level
    rows = size.row; // set sizes in level

    char** p_mapArray = new char* [size.row];
    for (int i = 0; i < size.row; i++)
    {
        p_mapArray[i] = new char[size.col];
    }

    int row = 0, col = 0, index = 0;

    // iterate through the characters of the map string
    for (string::size_type i = 0; i < map.size(); i++)
    {
        row = abs((int)i / size.col);
        col = size.col - ((((row + 1) * size.col) - (int)i));

        // add to dynamic map array
        p_mapArray[row][col] = map[i];

    }

    // return map array pointer
    p_map = p_mapArray;
}

string Scene::LoadSceneFromFile(string filename, int scene_to_load)
{
    
    ifstream scenesFile(filename);
    string fileLine, section, map = "";
    bool processLines = true;

    if (scenesFile) {
        // file exists and is open 
        while (getline(scenesFile, fileLine))
        {

            //check for right scene
            section = "#scene:";
            if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) { // if line is scene identifier

                if (scene_to_load == stoi(fileLine.substr(Utility::TransformString(section, 1).size(), 2))) // and if the sence is the one I'm looking for
                {
                    
                    // set the scene number
                    this_scene = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), 2));
                    
                    //this is the scene we are looking for so let's process it until your get to the end of the level info
                    while (getline(scenesFile, fileLine))
                    {
                        // get the next line and set the relevant info in the level info

                        section = "title:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            title = fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size()));
                            continue;
                        }

                        section = "pellet_points:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            points_ghost = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "ghost_points:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            points_ghost = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "all_ghosts_bonus:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            all_ghost_bonus = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "chase_duration:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            chase_for = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "run_duration:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            run_for = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "roam_duration:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            roam_for = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "roam_count:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            roam_count = stoi(fileLine.substr(Utility::TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "level_map:";
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            // get map info into the map string ending when you reach end of map
                            section = "#end_scene";
                            while (getline(scenesFile, fileLine))
                            {
                                if (fileLine.find(Utility::TransformString(section, 1), 0) == std::string::npos)
                                    map += fileLine;
                                else
                                    break;
                            }
                        }

                        section = "#scene:"; // check to see if reached the next scene;
                        if (fileLine.find(Utility::TransformString(section, 1), 0) != std::string::npos) {
                            break;
                        }

                    }
                }
                processLines = false;
            }
        }
        scenesFile.close();
    }
    else
    {
        //unable to read file - write sine error code. For now not polished.
        system("cls");
        cout << "Can't seem to load the PacMan scenes. Check your code man!" << endl;
        system("pause");
        exit(0);
    }

    return (map.size() > 0 ? map : "false");
}

Coord Scene::MapSize(const string& map)
{
    size_t rows = 0, cols = 0;
    size_t pos;

    pos = map.find("+");
    if (pos != string::npos)
    {
        pos = map.find("+", pos + 1);
        if (pos != string::npos)
        {
            cols = pos + 1;
            pos = map.find("+", pos + 1);
            if (pos != string::npos) {
                rows = (pos / cols) + 1;
            }
        }
    }
    return { (int)rows, (int)cols };
}

void Scene::DrawLevel()
{
    Draw::ShowConsoleCursor(false);
    
    // place cursor on top left of console
    Draw::CursorTopLeft(rows + top_left_modifier); // + x for cpations

    // update width and height of the level
    ResizeScene();
  
    // Level Title
    string format = Utility::Spacer(" PACMAN: " + Utility::TransformString(title, 0), cols);
    Draw::SetColor(Globals::cWHITE);
    cout << endl;
    cout << format << "PACMAN: " + Utility::TransformString(title, 0) << format;
    cout << endl << Draw::WriteEmptyLine(cols + 10) << endl;

    total_pellets = 0;

    // remove cursor and replace with current content of the cursor
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (p_map[r][c] == char(Globals::cursor))
            {
                if (p_editor->p_cursor->Pen() == '0' || !p_editor->p_cursor->IsEditing())
                {
                    p_map[r][c] = p_editor->p_cursor->content_now;
                }
                else
                {
                    p_map[r][c] = p_editor->p_cursor->Pen();
                }
               
            }
        }
    }

    // check pen is valid (will set to '.' if not)
    if (!PenIsValid(p_editor->p_cursor->Pen()))
    {
        p_editor->p_cursor->SetPen('.');
    }

    // draw current level map
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {

            // if this is the cursor current pos place it there
            if (p_editor->p_cursor->GetCurrentPosition().IsSame(Coord{ r,c }))
            {
                p_editor->p_cursor->content_now = p_map[p_editor->p_cursor->GetCurrentRow()][p_editor->p_cursor->GetCurrentCol()];
                p_map[r][c] = char(Globals::cursor);
            }
                
            
            // set color of map content at this position
            switch (p_map[r][c])
            {
            case 'o':
            case Globals::player_start:
            case Globals::ghost_spawn_target:
            case Globals::invisible_wall: // % = invisible wall
            case Globals::teleport: // T = portal
            case Globals::space: // ' ' = empty space
            case Globals::one_way: // $ = one way door for ghost spawn area
                Draw::SetColor(Globals::cWHITE); // black on black = not visible
                break;
            case (char)Globals::pellet: // . = pellet
                total_pellets++;
                Draw::SetColor(Globals::cWHITE); // gray for bullets
                break;
            case (char)Globals::powerup: // o = power up
                total_pellets++;
                Draw::SetColor(Globals::cWHITE); // white for power ups
                break;
            case Globals::pink_ghost: // blue ghost
                Draw::SetColor(479);
                break;
            case Globals::yellow_ghost: // yellow ghost
                Draw::SetColor(367);
                break;
            case Globals::blue_ghost: // green ghost
                Draw::SetColor(435);
                break;
            case Globals::red_ghost: // red ghost
                Draw::SetColor(71);
                break;
            case char(Globals::cursor): // red ghost
                if (!p_editor->p_cursor->IsEditing()) { // flashing effect - signals edible sate of ghost
                    p_editor->p_cursor->Blink() ? Draw::SetColor(Globals::c_blackwhite) : Draw::SetColor(Globals::c_whiteblack);
                    p_editor->p_cursor->SetBlink();
                }
                else { // solid color
                    Draw::SetColor(Globals::c_bluewhite);
                }
                break;
            default:
                Draw::SetColor(Globals::cWALLS); // gray bg on gray text for all other chars making them walls essentially
                break;
            }

            // print char
            cout << p_map[r][c];

            // set color back to default
            Draw::SetColor(Globals::cWHITE);

            // write a space to delete resize left over
            cout << (c == cols - 1 ? " " : "");
        }
        // end of row ad line feed
        cout << endl;
        cout << (r == rows -1 ? Draw::WriteEmptyLine(cols + 15) : "");
        
        
    }

    ShowKey();
    cout << Draw::WriteEmptyLine(cols+15);

} 

int Scene::AddRemoveColumns(bool add)
{
     // set max and min bouderies for size of the level
    if ((!add && cols - 1 < Globals::min_scene_cols) || (add && cols + 1 > Globals::max_scene_cols))
    {
        // exit add/remove
        return 0;
    }

    // determine where to add/remove from
    bool left = cols_from_left;
    int new_cols = add ? cols + 1 : cols - 1;

    // create temp array with the new scene size
    char** p_map_new = new char* [rows];
    for (int i = 0; i < rows; i++)
    {
        p_map_new[i] = new char[new_cols];
    }

    // place pellets in the entire array
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < new_cols; col++)
        {
            p_map_new[row][col] = char(Globals::pellet);
        }
    }

    // insert from current array into new one
    int iCols = add ? cols : cols - 1;
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < iCols; col++)
        {
            if (add)
            {
                p_map_new[row][left ? col : col + 1] = p_map[row][col];
            }
            else
            {
                p_map_new[row][col] = p_map[row][left ? col : col + 1];
            }
        }
    }

    // shift rows
    if (add)
    {
        // shift the walls left or right and set pellets in preior state
        for (int row = 0; row < rows; row++)
        {
            p_map_new[row][left ? new_cols - 1 : 0] = p_map[row][left ? cols - 1 : 0];
            p_map_new[row][left ? new_cols - 2 : 1] = char(Globals::pellet);
        }

        p_map_new[0][left ? new_cols - 2 : 1] = '-';
        p_map_new[rows - 1][left ? new_cols - 2 : 1] = '-';
    }
    else
    {
        // shift the walls left or right and set pellets in preior state
        for (int row = 0; row < rows; row++)
        {
            p_map_new[row][left ? new_cols - 1 : 0] = p_map[row][left ? cols - 1 : 0];
        }
    }


    // place cursor on top left of console
    Draw::CursorTopLeft(rows + top_left_modifier); // + x for cpations

    // clean up screen
    // system("cls");
    
    // delete the current p_map
    DeallocateMapArray();

    // set the p_map to the new array
    p_map = p_map_new;
    cols = new_cols;

    // set the new aray to null
    p_map_new = nullptr;

    // set flag fromleft/right to opposite
    cols_from_left = !cols_from_left;

    //clean exit
    return 0;

}

int Scene::AddRemoveRows(bool add)
{
    // set max and min bouderies for size of the level
    if ((!add && rows - 1 < Globals::min_scene_rows) || (add && rows + 1 > Globals::max_scene_rows))
    {
        return 0;
    }
    
    // determine where to add/remove from
    bool left = rows_fom_bottom;
    int new_rows = add ? rows + 1 : rows - 1;

    // create temp array with the new scene size
    char** p_map_new = new char* [new_rows];
    for (int i = 0; i < new_rows; i++)
    {
        p_map_new[i] = new char[cols];
    }

    // place pellets in the entire new array
    for (int row = 0; row < new_rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            p_map_new[row][col] = char(Globals::pellet);
        }
    }

    // insert from current array into new one
    int iRows = add ? rows : rows - 1;
    for (int row = 0; row < iRows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            if (add)
            {
                p_map_new[left ? row : row + 1][col] = p_map[row][col];
            }
            else
            {
                p_map_new[row][col] = p_map[left ? row : row + 1][col];
            }
        }
    }


    // shift rows
    if (add)
    {
        // shift the walls left or right and set pellets in preior state
        for (int col = 0; col < cols; col++)
        {
            p_map_new[left ? new_rows - 1 : 0][col] = p_map[left ? rows - 1 : 0][col];
            p_map_new[left ? new_rows - 2 : 1][col] = char(Globals::pellet);
        }

        p_map_new[left ? new_rows - 2 : 1][0] = '|';
        p_map_new[left ? new_rows - 2 : 1][cols - 1] = '|';
    }
    else
    {
        // shift the walls left or right and set pellets in preior state
        for (int col = 0; col < cols; col++)
        {
            p_map_new[left ? new_rows - 1 : 0][col] = p_map[left ? rows - 1 : 0][col];
        }
    }

    // place cursor on top left of console
    Draw::CursorTopLeft(rows + top_left_modifier); // + x for cpations

    // delete the current p_map
    DeallocateMapArray();

    // set the p_map to the new array
    p_map = p_map_new;
    rows = new_rows;

    // set the new aray to null
    p_map_new = nullptr;

    // set flag fromleft/right to opposite
    rows_fom_bottom = !rows_fom_bottom;

    // clean exit
    return 0;
}

void Scene::ResizeScene()
{

    if (add_cols)
    {
        AddRemoveColumns(true);
        add_cols = false;
    }

    if (add_rows)
    {
        AddRemoveRows(true);
        add_rows = false;
    }

    if (remove_cols)
    {
        AddRemoveColumns(false);
        remove_cols = false;
    }

    if (remove_rows)
    {
        AddRemoveRows(false);
        remove_rows = false;
    }

    //// display menu with rowws and columns options
    //string resize_options[4][2]{};
    //MainMenu resize_menu;
    //int new_row_size = rows, new_col_size = cols;

    //do
    //{
    //    system("cls");
    //    resize_options[0][0] = "#rows";
    //    resize_options[0][1] = "Rows (current scene " + to_string(rows) + "): " + to_string(new_row_size);
    //    resize_options[1][0] = "#cols";
    //    resize_options[1][1] = "Columns (current scene " + to_string(cols) +"): " + to_string(new_col_size);
    //    resize_options[2][0] = "#save";
    //    resize_options[2][1] = "SAVE";
    //    resize_options[3][0] = "#cancel";
    //    resize_options[3][1] = "CANCEL";
    //    resize_menu.Create(resize_options, 4);
    //    resize_menu.Template(MenuTemplates::EDIT_SCENE_OPTIONS);
    //    string selection = resize_menu.Show();

    //    if (selection == "#rows")
    //    {
    //        new_row_size = ProcessNumberOption("Set rows to: ");
    //    }
    //    else if (selection == "#cols")
    //    {
    //        new_col_size = ProcessNumberOption("Set columns to: ");
    //    }
    //    else if (selection == "#save")
    //    {
    //        //updated array and rows
    //        ResizeMap(new_row_size, new_col_size);
    //        break;
    //    }
    //    else
    //    {
    //        // exit with no update
    //        break;
    //    }


    //} while (true);
    

}

// setters
void Scene::SetEditor(Editor* p_editor)
{
    this->p_editor = p_editor;
}

void Scene::ShowKey()
{
    string format = "";
    if (error_count > 0)
    {
        format = Utility::Spacer(scene_errors, cols);
        cout << endl;
        cout << Draw::WriteEmptyLine(70) << '\r';
        cout << format << scene_errors;
        cout << Draw::WriteEmptyLine(70) << '\r';
        top_left_modifier = 5;

        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); // pause to slow game

        scene_errors = "";
        error_count = 0;
    }
    else if (resize_scene)
    {
        format = Utility::Spacer("Left/Right arrows: +/- Columns (  )", cols);
        string format2 = Utility::Spacer("Down/Up arrows: +/- Rows (  )", cols);
        cout << endl;
        cout << Draw::WriteEmptyLine(70) << '\r';
        cout << format << "Left/Right arrows: +/- Columns (" + to_string(cols) + ")" << format << endl;
        cout << Draw::WriteEmptyLine(70) << '\r';
        cout << format2 << " Down/Up arrows: +/- Rows (" + to_string(rows) + ")" << format2 << endl;
        top_left_modifier = 6;
    }
    else if (p_editor->p_cursor->IsEditing())
    {
        if (p_editor->p_cursor->Pen() == '0' && !resize_scene)
        {
            cout << endl;
            cout << Draw::WriteEmptyLine(70) << '\r';
            cout << " '.' pellet, 'o' powerup, ' ' space, 'S' Player start, 'R' resize" << endl;
            cout << Draw::WriteEmptyLine(70) << '\r';
            cout << " '#' wall, '%' hidden wall, 'T' teleport, '^' Ghost spawn" << endl;
            top_left_modifier = 6;
        }
        else
        {
            format = Utility::Spacer("' ' pen. 'Esc' to swap. 'Space' disables.", cols);
            cout << endl;
            cout << Draw::WriteEmptyLine(70) << '\r';
            cout << format << "'" << p_editor->p_cursor->Pen() << "' pen. 'Esc' to swap. 'Space' disables." << format << endl;
            cout << Draw::WriteEmptyLine(70) << '\r';
            top_left_modifier = 6;
        }
        
    }
    else if (p_editor->IsDoneEditing())
    {
        format = Utility::Spacer("Save? 'y' yes, 'n' no, 'esc' cancel", cols);
        cout << endl;
        cout << Draw::WriteEmptyLine(70) << '\r';
        cout << format << "Save? 'y' yes, 'n' no, 'esc' cancel" << format << endl;
        cout << Draw::WriteEmptyLine(70) << '\r';
        top_left_modifier = 5;
    }
    else
    {
        format = Utility::Spacer("'Space' to select pen. 'Esc' when done.", cols);
        cout << endl;
        cout << Draw::WriteEmptyLine(70) << '\r';
        cout << format << "'Space' to select pen. 'Esc' when done." << format << endl;
        cout << Draw::WriteEmptyLine(70) << '\r';
        top_left_modifier = 5;
    }
}

string** Scene::GetSceneValues()
{
    
    // if array of options already exists, delete it
    DeleteOptions();
    
    // create the options array
    p_options = new string * [11];
    for (int i = 0; i < 11; i++)
    {
        p_options[i] = new string[2];
    }
    p_options[0][0] = "0";
    p_options[0][1] = "Scene: " + to_string(this_scene);

    p_options[1][0] = "1";
    p_options[1][1] = "Title: " + title;

    p_options[2][0] = "2";
    p_options[2][1] = "Points p pellet: " + to_string(points_pellet);

    p_options[3][0] = "3";
    p_options[3][1] = "Points p ghots: " + to_string(points_ghost);

    p_options[4][0] = "4";
    p_options[4][1] = "Points all ghosts: " + to_string(all_ghost_bonus);

    p_options[5][0] = "5";
    p_options[5][1] = "Chase mode duration: " + to_string(chase_for);
    
    p_options[6][0] = "6";
    p_options[6][1] = "Run mode duration: " + to_string(run_for);
    
    p_options[7][0] = "7";
    p_options[7][1] = "Roam mode duration: " + to_string(roam_for);

    p_options[8][0] = "8";
    p_options[8][1] = "Roam count: " + to_string(roam_count);

    p_options[9][0] = "#save";
    p_options[9][1] = "SAVE";

    p_options[10][0] = "#cancel";
    p_options[10][1] = "CANCEL";

    return p_options;
 }

void Scene::DeleteOptions()
{
    if (p_options)
    {
        for (int i = 0; i < 10; i++)
        {

            delete[] p_options[i];
            p_options[i] = nullptr;
        }
        delete[] p_options;
        p_options = nullptr;

    }
}

bool Scene::PenIsValid(char pen)
{
    int count = 0;
    int max_count = 0;
    switch (pen)
    {
    case 'T':
        max_count = 2;
        break;
    case 'S':
    case '^':
        max_count = 1;
        break;
    default:
        return true;
    }

    // remove cursor and replace with current content of the cursor
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (p_map[r][c] == pen)
            {
                count++;
            }
        }
    }

    return (count > max_count ? false : true);
}

bool Scene::SaveToFile()
{
    

    // get the text from file until we need to insert the update
    ifstream read_scenes("PacMan.Scenes");
    string file_line, text_1 = "", text_2 = "", marker;
    bool is_part2 = false;
    int scene_number = 0;

    if (read_scenes)
    {
        // file exists and is open 
        while (getline(read_scenes, file_line))
        {
            marker = "#scene:";

            if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos)
            {
                scene_number++;
            }

            marker = "#scene:" + to_string(this_scene);

            if (file_line.find(Utility::TransformString(marker, 1), 0) == std::string::npos) {
                if (!is_part2) {
                    text_1 += file_line + "\n";
                }
                else {
                    text_2 += file_line + "\n";
                }
                continue;
            }
            else
            {
                is_part2 = true;
                
                if (this_scene == 0)
                {
                    // place the scene into text_2
                    text_2 += "\n" + file_line + "\n";
                    while (getline(read_scenes, file_line))
                    {
                        marker = "#end_scene";
                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos)
                        {
                            text_2 += file_line + "\n";
                            break; // skip
                        }
                        else
                        {
                            text_2 += file_line + "\n";
                        }
                    }
                }
                else
                {
                    // move past the scene
                    while (getline(read_scenes, file_line))
                    {
                        marker = "#end_scene";
                        if (file_line.find(Utility::TransformString(marker, 1), 0) == std::string::npos)
                        {
                            continue; // skip
                        }
                        else
                        {
                            break; // exit this loop
                        }
                    }

                }
                
                
            }

        }
        read_scenes.close();
    }

    // create the scene updates string
    string update = CreatesceneString(scene_number);

    update = text_1 + update + text_2;

    
    //std::ofstream outfile;
    //outfile.open("test.txt", std::ios_base::app); // append instead of overwrite
    //outfile << "Data";
    //return 0;
    
    ofstream scene_file("PacMan.scenes");
    scene_file << update;
    scene_file.close();

    /*string fileLine, section, map = "";
    bool processLines = true;*/

    return true;

}

string Scene::CreatesceneString(int scene)
{
    // create the string to write
    string map = "";
    string update = "";
    update += "#scene:" + to_string(this_scene != 0 ? this_scene : scene) + "\n";
    update += "title:" + title + "\n";
    update += "pellet_points:" + to_string(points_pellet) + "\n";
    update += "ghost_points:" + to_string(points_ghost) + "\n";
    update += "all_ghosts_bonus:" + to_string(all_ghost_bonus) + "\n";
    update += "chase_duration:" + to_string(chase_for) + "\n";
    update += "run_duration:" + to_string(run_for) + "\n";
    update += "roam_duration:" + to_string(roam_for) + "\n";
    update += "roam_count:" + to_string(roam_count) + "\n";
    update += "level_map:\n";
    // insert map array
    
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            if (p_map[row][col] == char(Globals::cursor))
            {
                p_map[row][col] = p_editor->p_cursor->content_now;
            }
            map += p_map[row][col];
        }
        map += "\n";
    }

    // set the boundry markers to '+' so we can get size of the map
    p_map[0][0] = '+';
    p_map[0][cols - 1] = '+';
    p_map[rows -1][0] = '+';
    p_map[rows -1][cols - 1] = '+';

    // replace the full stop with pellet char
    Utility::ReplaceString(map, char(Globals::pellet), '.');
    
    update += map;
    update += "#end_scene\n";

    return update;
}

bool Scene::ValiditeScene()
{
    int teleport = 0, player_start = 0, ghost_spawn_target = 0;
    bool no_dead_ends = true, has_outer_walls = true, is_valid_scene = true;
    scene_errors = "";

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            // do counts
            switch (p_map[row][col])
            {
            case char(Globals::teleport):
                teleport++;
                break;
            case char(Globals::player_start):
                player_start++;
                break;
            case char(Globals::ghost_spawn_target):
                ghost_spawn_target++;
                break;
            }

            // check for dead end
            if (!HasNoDeadEnd(row, col))
            {
                no_dead_ends = false;
            }

            // check for outer walls
            if (!HasOuterWalls(row, col))
            {
                has_outer_walls = false;
            }

        }
    }

     if (teleport > 2 || teleport < 2)
    {
        scene_errors = "You need one pair of teleports\n";
        is_valid_scene = false;
        error_count++;
    }

    if (player_start > 1 || player_start < 1)
    {
        scene_errors = "You need one player start position.\n";
        is_valid_scene = false;
        error_count++;
    }

    if (ghost_spawn_target > 1 || ghost_spawn_target < 1)
    {
        scene_errors = "You need one ghosts spawn target.\n";
        is_valid_scene = false;
        error_count++;
    }

    if (!no_dead_ends)
    {
        scene_errors = "The level cannot have dead ends.\n";
        is_valid_scene = false;
        error_count++;
    }

    if (!has_outer_walls)
    {
        scene_errors = "You need outer walls around the scene.\n";
        is_valid_scene = false;
        error_count++;
    }

    return is_valid_scene;

}

bool Scene::HasOuterWalls(int row, int col)
{
    if (col == 0 || col == cols || row == 0 || row == rows)
    {
        if (p_map[row][col] == '+' || p_map[row][col] == '|' || p_map[row][col] == '-' || p_map[row][col] == '#' || (p_map[row][col] == Globals::teleport))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool Scene::HasNoDeadEnd(int row, int col)
{
    // replace the cursor with the content from the sqaure its on
    p_map[p_editor->p_cursor->GetCurrentRow()][p_editor->p_cursor->GetCurrentCol()] = p_editor->p_cursor->content_now;
    
    // if in baounds of the outer walls
    if (col > 0 && col - 1 < cols && row > 0 && row < rows - 1)
    {
        
        // and the current position is a pellet
        if (p_map[row][col] == char(Globals::pellet))
        {
            // look around the sqaure for other pellet sqaures
            int route_count = 0; int this_row = 0, this_col = 0;
            for (int i = 0; i < 4; i++)
            {

                this_row = row, this_col = col;
                switch (i)
                {
                case 0: // above
                    this_row = row - 1;
                    break;
                case 1: // to right
                    this_col = col + 1;
                    break;
                case 2: // below
                    this_row = row + 1;
                    break;
                case 3: // to left
                    this_col = col - 1;
                    break;
                }

                
                switch (p_map[this_row][this_col])
                {
                case char(Globals::space):
                case char(Globals::pellet):
                case char(Globals::powerup):
                case 'o':
                case char(Globals::player_start):
                case char(Globals::ghost_spawn_target):
                    route_count++;
                    break;
                default:
                    break;
                }

            }

            // if the number of pellets around the sqaure is more than 1 there are no dead ends
            return route_count > 1 ? true : false;

        }

        return true;
    }

    return true;
}

void Scene::DeallocateMapArray()
{
    // deallocated map array
    if (p_map)
    {
        for (int i = 0; i < rows; i++) {
            delete[] p_map[i];
        }
        delete[] p_map;
        p_map = nullptr;
    }

}
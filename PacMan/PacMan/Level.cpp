#include <fstream> // file manipulation
#include <iostream> // print to console
#include <stdlib.h> // exit
#include <conio.h> // used to get input without requiring return
#include "EnumsAndStatics.h"
#include "Utility.h"
#include "Draw.h"
#include "Level.h"
#include "Game.h"

using namespace std;
using namespace std::chrono;

//constructors
Level::Level()
{
	// do nothing for now
}

//destructors
Level::~Level()
{
    // deallocate the dynamic level array
    if (p_map)
    {
        for (int i = 0; i < rows; i++) {
            delete[] p_map[i];
        }
        delete[] p_map;
        p_map = nullptr;
    }

    if (p_game)
    {
        p_game = nullptr;
    }
}

// Public Methods
void Level::SetupLevel(int& current_scene)
{
    // reset initial state of key level variables
    level_paused = true;
    level_paused = true;
    level_mode = Mode::CHASE;
    eaten_pellets = 0;
    eaten_ghosts = 0;
    all_eaten_ghosts = 0;
    roam_count = 0;
    is_complete = false;

    // create the scene map and set up player + ghosts accordingly
    CreateLevelScene(current_scene);

    // start the chase timer
    chase_time_start = chrono::high_resolution_clock::now();
}

void Level::CreateLevelScene(int& current_scene)
{
    Utility utility;

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
    utility.ReplaceString(map, ".", char(Globals::pellet));
    utility.ReplaceString(map, "o", char(Globals::powerup));

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
        row == NULL ? row = 0 : row = row; // this fixed it

        // add to dynamic map array
        p_mapArray[row][col] = map[i];

        // count pellets that determine end game condition
        if (map[i] == (char)Globals::pellet || map[i] == (char)Globals::powerup)
            total_pellets++;

        // Set player start to position 'S'
        if (map[i] == Globals::player_start)
        {
            p_game->p_player->SetPositions(row, col);
        }

        // Set ghost spawn target location
        if (map[i] == Globals::ghost_spawn_target)
        {
            ghost_spawn.row = row;
            ghost_spawn.col = col;
            p_mapArray[row][col] = (char)Globals::pellet;
            total_pellets++;

            // set ghosts spawn target
            for (int g = 0; g < Globals::total_ghosts; g++) // loop through ghosts
            {
                p_game->p_ghosts[g]->SetSpawnTarget(row, col);
            }
        }

        // Set Ghost start position
        int index = -1;
        switch (map[i])
        {
        case Globals::red_ghost:
            index = 0;
            break;
        case Globals::yellow_ghost:
            index = 1;
            break;
        case Globals::blue_ghost:
            index = 2;
            break;
        case Globals::pink_ghost:
            index = 3;
            break;
        }
        if (index >= 0 && index < 4) {
            p_game->p_ghosts[index]->SetSpawnPosition(row, col);
            p_game->p_ghosts[index]->SetCurrentPosition(row, col);
            p_game->p_ghosts[index]->SetPreviousPosition(row, col);
        }


        // Set teleport coords for T1 and T2
        if (map[i] == Globals::teleport) {
            if (row == 0 || col == 0) {
                tp_1 = { row, col };
            }
            else {
                tp_2 = { row, col };
            }
            // set teleport direction
            if (col == 0) {
                tp_row = true; // teleport across
            }
            else if (row == 0) {
                tp_row = false; // teleport certically
            }
        }
    }

    // return map array pointer
    p_map = p_mapArray;
}

string Level::LoadSceneFromFile(string filename, int scene_to_load)
{
    Utility utility; // access to utilities
    
    ifstream scenesFile(filename);
    string fileLine, section, map = "";
    bool processLines = true;

    if (scenesFile) {
        // file exists and is open 
        while (getline(scenesFile, fileLine))
        {

            //check for right scene
            section = "#scene:";
            if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) { // if line is scene identifier

                if (scene_to_load == stoi(fileLine.substr(utility.TransformString(section, 1).size(), 2))) // and if the sence is the one I'm looking for
                {
                    //this is the scene we are looking for so let's process it until your get to the end of the level info
                    while (getline(scenesFile, fileLine))
                    {
                        // get the next line and set the relevant info in the level info

                        section = "title:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            title = fileLine.substr(utility.TransformString(section, 1).size(), (fileLine.size() - section.size()));
                            continue;
                        }

                        section = "pellet_points:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            points_ghost = stoi(fileLine.substr(utility.TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "ghost_points:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            points_ghost = stoi(fileLine.substr(utility.TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "all_ghosts_bonus:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            all_ghost_bonus = stoi(fileLine.substr(utility.TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "edible_ghost duration:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            edible_ghost_duration = stoi(fileLine.substr(utility.TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "chase_duration:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            chase_for = stoi(fileLine.substr(utility.TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "run_duration:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            run_for = stoi(fileLine.substr(utility.TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "roam_duration:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            roam_for = stoi(fileLine.substr(utility.TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "roam_count:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            roam_count = stoi(fileLine.substr(utility.TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "level_map:";
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
                            // get map info into the map string ending when you reach end of map
                            section = "#end_scene";
                            while (getline(scenesFile, fileLine))
                            {
                                if (fileLine.find(utility.TransformString(section, 1), 0) == std::string::npos)
                                    map += fileLine;
                                else
                                    break;
                            }
                        }

                        section = "#scene:"; // check to see if reached the next scene;
                        if (fileLine.find(utility.TransformString(section, 1), 0) != std::string::npos) {
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

Coord Level::MapSize(const string& map)
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

void Level::DrawLevel()
{
    Draw draw;
    Utility utility;

    // set the content of the sqaure the player is moving into - will use this to play the appropriate sound
    p_game->p_player->SetMovedIntoSquareContents(p_map[p_game->p_player->GetCurrentRow()][p_game->p_player->GetCurrentCol()]);

    int current_pellets = 0; // bug fix - loosing some pellets on map - will need to perma fix but this will do for now

    // set the content of the sqaure the ghosts are currently back to what they moved over (pellet, power up or space)
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            switch (p_map[r][c])
            {
            case Globals::red_ghost:
                p_map[r][c] = p_game->p_ghosts[0]->GetPreviousSqaureContent();
            case Globals::yellow_ghost:
                p_map[r][c] = p_game->p_ghosts[1]->GetPreviousSqaureContent();
            case Globals::blue_ghost:
                p_map[r][c] = p_game->p_ghosts[2]->GetPreviousSqaureContent();
            case Globals::pink_ghost:
                p_map[r][c] = p_game->p_ghosts[3]->GetPreviousSqaureContent();
            }
        }
    }

    // place cursor on top left of console
    draw.CursorTopLeft(rows + 5); // + 5 for title and status

    // remove player from map at last position if they are different
    if (!p_game->p_player->GetPreviousPosition().IsSame(p_game->p_player->GetCurrentPosition()))
        p_map[p_game->p_player->GetPreviousRow()][p_game->p_player->GetPreviousCol()] = Globals::space;

    // player in tunnel
    if (p_game->p_player->GetCurrentPosition().IsSame(tp_1)) {
        p_map[p_game->p_player->GetCurrentRow()][p_game->p_player->GetCurrentCol()] = Globals::teleport;
        p_game->p_player->SetCurrentPosition(tp_2);
    }
    else if (p_game->p_player->GetCurrentPosition().IsSame(tp_2)) {
        p_map[p_game->p_player->GetCurrentRow()][p_game->p_player->GetCurrentCol()] = Globals::teleport;
        p_game->p_player->SetCurrentPosition(tp_1);
    }

    for (int g = 0; g < Globals::total_ghosts; g++) // loop through ghots
    {
        // remove ghosts from map at last position if they are different
        if (p_game->p_ghosts[g]->GetPreviousPosition().IsSame(p_game->p_ghosts[g]->GetCurrentPosition()))
            p_map[p_game->p_ghosts[g]->GetPreviousRow()][p_game->p_ghosts[g]->GetPreviousCol()] = Globals::space;

        // ghost in tunnel
        if (!p_game->p_ghosts[g]->SkipTurn())
        {
            if (p_game->p_ghosts[g]->GetCurrentPosition().IsSame(tp_1)) {
                p_map[p_game->p_ghosts[g]->GetCurrentRow()][p_game->p_ghosts[g]->GetCurrentCol()] = Globals::teleport;
                p_game->p_ghosts[g]->SetCurrentPosition(tp_2);
            }
            else if (p_game->p_ghosts[g]->GetCurrentPosition().IsSame(tp_2)) {
                p_map[p_game->p_ghosts[g]->GetCurrentRow()][p_game->p_ghosts[g]->GetCurrentCol()] = Globals::teleport;
                p_game->p_ghosts[g]->SetCurrentPosition(tp_1);
            }
        }

    }

    // Level Title
    string format = utility.Spacer(" PACMAN: " + utility.TransformString(title, 0), cols);
    draw.SetColor(Globals::cWHITE);
    cout << endl;
    cout << format;
    cout << "PACMAN: " << utility.TransformString(title, 0);
    cout << format;
    cout << endl << endl;

    // draw current level map
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            //Character* characters[5] = { Player, ghost1 };

            //characters[0]->DrawCharacter();

            // position player
            if (p_game->p_player->GetCurrentPosition().IsSame(Coord(r, c)))
                p_map[r][c] = Globals::player;

            for (int g = 0; g < Globals::total_ghosts; g++) // loop through ghots
            {
                // position ghost
                if (p_game->p_ghosts[g]->GetCurrentPosition().IsSame(Coord(r, c)))
                    p_map[r][c] = p_game->p_ghosts[g]->GhostChar();

                // if the ghost moved from the last position
                if (!p_game->p_ghosts[g]->GetPreviousPosition().IsSame(p_game->p_ghosts[g]->GetCurrentPosition()))
                {
                    // put back the content of the square the ghost was last at
                    if (p_game->p_ghosts[g]->GetPreviousPosition().IsSame(Coord(r, c)))
                        p_map[r][c] = p_game->p_ghosts[g]->GetPreviousSqaureContent();
                }
            }

            // set color of map content at this position
            switch (p_map[r][c])
            {
            case Globals::invisible_wall: // % = invisible wall
            case Globals::teleport: // T = portal
            case Globals::space: // ' ' = empty space
            case Globals::one_way: // $ = one way door for ghost spawn area
                draw.SetColor(Globals::cINVISIBLE); // black on black = not visible
                break;
            case (char)Globals::pellet: // . = pellet
                current_pellets++;
                draw.SetColor(Globals::cWHITE); // gray for bullets
                break;
            case (char)Globals::powerup: // o = power up
                current_pellets++;
                draw.SetColor(Globals::cWHITE); // white for power ups
                break;
            case Globals::pink_ghost: // blue ghost
                if (p_game->p_ghosts[3]->IsEdible()) { // flashing effect - signals edible sate of ghost
                    p_game->p_ghosts[3]->FlashBlue() ? draw.SetColor(Globals::cGHOST_ON) : draw.SetColor(Globals::cGHOST_OFF);
                    p_game->p_ghosts[3]->SetFlashBlue(!p_game->p_ghosts[0]->FlashBlue());
                }
                else { // solid color
                    draw.SetColor(p_game->p_ghosts[3]->GetColor());
                }
                break;
            case Globals::yellow_ghost: // yellow ghost
                if (p_game->p_ghosts[1]->IsEdible()) { // flashing effect - signals edible sate of ghost
                    p_game->p_ghosts[1]->FlashBlue() ? draw.SetColor(Globals::cGHOST_ON) : draw.SetColor(Globals::cGHOST_OFF);
                    p_game->p_ghosts[1]->SetFlashBlue(!p_game->p_ghosts[0]->FlashBlue());
                }
                else { // solid color
                    draw.SetColor(p_game->p_ghosts[1]->GetColor());
                }
                break;
            case Globals::blue_ghost: // green ghost
                if (p_game->p_ghosts[2]->IsEdible()) { // flashing effect - signals edible sate of ghost
                    p_game->p_ghosts[2]->FlashBlue() ? draw.SetColor(Globals::cGHOST_ON) : draw.SetColor(Globals::cGHOST_OFF);
                    p_game->p_ghosts[2]->SetFlashBlue(!p_game->p_ghosts[0]->FlashBlue());
                }
                else { // solid color
                    draw.SetColor(p_game->p_ghosts[2]->GetColor());
                }
                break;
            case Globals::red_ghost: // red ghost
                if (p_game->p_ghosts[0]->IsEdible()) { // flashing effect - signals edible sate of ghost
                    p_game->p_ghosts[0]->FlashBlue() ? draw.SetColor(Globals::cGHOST_ON) : draw.SetColor(Globals::cGHOST_OFF);
                    p_game->p_ghosts[0]->SetFlashBlue(!p_game->p_ghosts[0]->FlashBlue());
                }
                else { // solid color
                    draw.SetColor(p_game->p_ghosts[0]->GetColor());
                }
                break;
            case Globals::player: // player
                draw.SetColor(Globals::cPLAYER);
                break;
            default:
                draw.SetColor(Globals::cWALLS); // gray bg on gray text for all other chars making them walls essentially
                break;
            }

            // print char
            cout << p_map[r][c];

            // set color back to default
            draw.SetColor(Globals::cWHITE);
        }
        // end of row ad line feed
        cout << endl;
    }

    // ********
    // BUG FIX - brute force - need to look at why we are dropping a few pellets
    eaten_pellets = eaten_pellets + (total_pellets - eaten_pellets - current_pellets);

}

bool Level::NotWall(const Coord& move, const Direction& direction)
{
    switch (p_map[move.row][move.col])
    {
    case Globals::space:
    case Globals::teleport:
    case (char)Globals::pellet:
    case (char)Globals::powerup:
    case Globals::red_ghost:
    case Globals::yellow_ghost:
    case Globals::blue_ghost:
    case Globals::pink_ghost:
    case Globals::player:
        return true;
        break;
    case Globals::one_way: // one way move to exit ghost spawn area
        return (direction != Direction::DOWN ? true : false);
        break;
    }
    return false;
}

void Level::CheckLevelComplete()
{
    eaten_pellets >= total_pellets ? is_complete = true : is_complete = false; // if all pellets are eaten game ends - 339 in this map
}

bool Level::IsTeleport(const Coord& move)
{
    if (tp_1.row == move.row && tp_1.col == move.col)
        return true;
    if (tp_2.row == move.row && tp_2.col == move.col)
        return true;
    return false;
}

// getters
bool Level::GameRefIsSet()
{
    return (p_game ? true : false);
}

// setters
void Level::SetGameRef(Game* p_game)
{
    this->p_game = p_game;
}
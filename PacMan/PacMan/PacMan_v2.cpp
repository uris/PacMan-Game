#pragma region Includes, Namespaces
#include <iostream> // base input / output
#include <string> // manipulate level string
#include <Windows.h> // used to output colors
#include <mmsystem.h> // sounds
#include <conio.h> // used to get input without requiring return
#include <thread> // used to get input while running game
#include <chrono> // used for timers
#include <iomanip> // formating output
#include <algorithm> // string transforms
#include <fstream> // file manipulation
#include <stdlib.h> // exit
#include "Enums.h"
#include "Coord.h"
#include "Draw.h"
#include "Character.h"
#include "Player.h"
#include "Ghost.h"
#include "Game.h"

using namespace std;
using namespace std::chrono;
#pragma endregion

//struct Ghost {
//    char ghost = 'R';
//    char square_content_now = ' ';
//    char square_content_prior = square_content_now;
//    Coord current_pos, spawn_pos, previous_pos;
//    Coord spawn_target; // just above spawn area
//    Coord roam_target; // roams to top right hand side
//    Coord chase_modifier; // runs after player position
//    Direction current_direction = Direction::UP;
//    Direction previous_direction = Direction::UP;
//    Mode mode = Mode::CHASE;
//    bool run_first_move = true;
//    bool is_edible = false;
//    bool color_on = false;
//    int color = 455;
//    int wait = 0;
//    bool skip_turn = false; // use this to slow down moster if edible
//    int look_ahead = 5; // how far ahead the IA looks for player
//};

struct Level {

    // Colors
    static constexpr int cINVISIBLE = 0; // black on black
    static constexpr int cWHITE = 7; // white
    static constexpr int cPLAYER = 14; // yellow
    static constexpr int cWALLS = 392; // gray on gray text
    static constexpr int cGHOST_ON = 275; // for ghost flash
    static constexpr int cGHOST_OFF = 155; // for ghost flash

    // Ghosts
    static constexpr int gRED = 0;
    static constexpr int gYELLOW = 1;
    static constexpr int gBLUE = 2;
    static constexpr int gPINK = 3;

    // level chars
    static constexpr int powerup = 254; //extended set of ascii 
    static constexpr int pellet = 250; // extended set of ascii
    static constexpr char player_start = 'S'; // pellet ascii
    static constexpr char ghost_spawn_target = '^'; // pellet ascii
    static constexpr char space = ' '; // pellet ascii
    static constexpr char invisible_wall = '%'; // pellet ascii
    static constexpr char one_way = '$'; // pellet ascii
    static constexpr char teleport = 'T'; // pellet ascii
    static constexpr char red_ghost = 'R'; // pellet ascii
    static constexpr char yellow_ghost = 'Y'; // pellet ascii
    static constexpr char blue_ghost = 'B'; // pellet ascii
    static constexpr char pink_ghost = 'P'; // pellet ascii

    // level map and scene
    string title = "Scene 1";
    char** p_map = nullptr;
    int rows = 23;
    int cols = 47;
    bool level_paused = true;
    Coord ghost_spawn;

    // teleport coords and direction
    Coord tp_1, tp_2;
    //Coord tp_1 = { tp_1.row = 0, tp_1.col = 0 };
    //Coord tp_2 = { tp_2.row = 0, tp_2.col = 0 };
    bool tp_row = true;

    // level stats
    int total_pellets = 0; // totala level pellets
    int eaten_pellets = 0; // pellets consumed
    int eaten_ghosts = 0; // pellets consumed
    int all_eaten_ghosts = 0; // pellets consumed
    int points_pellet = 10;
    int points_ghost = 250;
    int all_ghost_bonus = 2600;
    bool is_complete = false;

    // Manange modes
    // bool ghosts_in_a_row[4]{false,false,false,false};
    Mode level_mode = Mode::CHASE;
    int edible_ghost_duration = 20; // seconds
    int roam_count = 8;
    int roam_for = 6; // seconds
    int chase_for = 16; // seconds
    int run_for = 15; // seconds
    high_resolution_clock::time_point roam_time_start = chrono::high_resolution_clock::now();
    high_resolution_clock::time_point chase_time_start = chrono::high_resolution_clock::now();
    high_resolution_clock::time_point run_time_start = chrono::high_resolution_clock::now();
    duration<double> roam_duration = duration_cast<duration<double>>(roam_time_start - chrono::high_resolution_clock::now());
    duration<double> chase_duration = duration_cast<duration<double>>(chase_time_start - chrono::high_resolution_clock::now());
    duration<double> run_duration = duration_cast<duration<double>>(run_time_start - chrono::high_resolution_clock::now());

};

#pragma region Functions
// Functions -->

// level setup, display and management
void SetUp(Game& game, Level& level, Player& player, Ghost ghosts[]);
char** CreateLevelScene(Game& game, Level& level, Player& player, Ghost ghosts[]);
void DrawLevel(Game& game, Level& level, Player& player, Ghost ghosts[]);
void StatusBar(Game& game, Level& level, Player& player);
void SpawnMonster(const Level& level, Ghost ghosts[]);
void SpawnMonster(const Level& level, Ghost ghosts[], Ghost ghost, const bool playerDied);
int SFX(Game& game, Level& level, Play playSFX);
string LoadSceneFromFile(string filename, int scene, Level& level);
//void ClearEatenGhosts(Level& level);
//bool AllGhostsEaten(Level& level);

// Player movement
void GetPlayerDirection(Game& game);
void MovePlayer(Level& level, Player& player, Ghost ghosts[]);

// Ghost movement
int MoveGhost(Game& game, Level& level, Player& player, Ghost& ghost, Ghost ghosts[]);
void DoGhostMove(Level& level, Player& player, Ghost ghosts[], Ghost& ghost, Direction bestMove);
int GetBestMove(Level& level, Player& player, Ghost& ghost, Coord move, Direction curr_direction, int depth, bool isGhost);
Direction RandomGhostMove(const Level& level, Player& player, Ghost& ghost);
char GetSquareContentNow(Level& level, Ghost ghosts[], Coord square);
Coord GhostTeleport(Level& level, Ghost& ghost, Coord move, Direction direction);

//common
bool NotWall(const Level& level, const Player& player, const Coord& move, const Direction& direction);
bool IsTeleport(const Level& level, const Coord& move);
bool IsReverse(const Direction& curr_direction, const Direction& new_direction);

// Player/Ghost Events and Status
void PlayerMonsterCollision(Game& game, Level& level, Player& player, Ghost ghosts[]);
bool CheckCollision(Player& player, Ghost& ghost);
void SetPlayerState(Level& level, Player& player, Ghost ghosts[]);
void SetGhostMode(Level& level, Player& player, Ghost ghosts[]);

// Game management
void CheckLevelComplete(Level& level);
void RefreshDelay(Game& game, Level& level);
void PlayerMonsterCollisionDelay(Game& game, Level& level);
void DeallocateMem(Level& level);
bool NextLevelRestartGame(Game& game, Level& level);

// Helper and scaffolding
string TransformString(string text, int operation);
string GhostMode(Ghost& ghost);
void Credits(Game& game, Level& level);
void ReplaceString(string& text, string from, char to);
Coord MapSize(const string& map);

#pragma endregion

int main()
{

    // set up game objects
    Game game;
    Level level;
    Player player;
    Ghost redGhost(Ghosts::RED);
    Ghost yellowGhost(Ghosts::YELLOW);
    Ghost blueGhost(Ghosts::BLUE);
    Ghost pinkGhost(Ghosts::PINK);
    game.Add(player, redGhost, yellowGhost, blueGhost, pinkGhost);

    Draw draw;

    Credits(game, level); // show credits
    
    do
    {
        
        // Set up game and initialize data
        SetUp(game, level, player, ghosts);

        // remove cursor from view for less flicker during game
        draw.ShowConsoleCursor(false);

        // draw initial level state
        DrawLevel(game, level, player, ghosts);

        // start a thread to get input while the main program executes
        thread inputThread(GetPlayerDirection, ref(game)); // new the ref() wrapper to pass by ref into thread

        do
        {
            // move the player
            MovePlayer(level, player, ghosts);

            // process any player / ghost collision
            PlayerMonsterCollision(game, level, player, ghosts);

            // set ghost chase modes as approprite
            SetGhostMode(level, player, ghosts);

            //// move the ghost
            MoveGhost(game, level, player, redGhost, ghosts);

            // process any player / ghost collision again
            PlayerMonsterCollision(game, level, player, ghosts);

            // delay render if there's a collision
            PlayerMonsterCollisionDelay(game, level);

            // Draw the level current state
            DrawLevel(game, level, player, ghosts);

            // end condition for the gmae once user has eaten all pellets
            CheckLevelComplete(level);

            // display stats and number lives
            StatusBar(game, level, player);

            // introduce a wait for fast PC
            RefreshDelay(game, level);

        } while (!level.is_complete && !game.IsGameOver());

        SFX(game, level, Play::NONE);

        // when game ends, wait for the input thread rejoin the main thread
        inputThread.join();

        // continue to next level, play again or quit
        if(!NextLevelRestartGame(game, level))
            break;
        
        // bring cursor from the console back into view
        draw.ShowConsoleCursor(true);

    } while (true);

    return 0;
}

#pragma region level setup, display and management
void SetUp(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    // set player state
    game.IsGameOver() ? player.SetLives(3) : player.SetLives();
    player.ClearEatenGohsts();
    
    // initialize ghosts
    SpawnMonster(level, ghosts);

    // name level
    level.level_paused = true;
    switch (game.GetCurrentScene())
    {
    case 1:
        level.title = "Scene 1";
        break;
    case 2:
        level.title = "Scene 2";
        break;
    default:
        level.title = "Scene 1";
        break;
    }

    // set level initial state
    level.level_paused = true;
    level.level_mode = Mode::CHASE;
    level.eaten_pellets = 0;
    level.eaten_ghosts = 0;
    level.all_eaten_ghosts = 0;
    level.roam_count = 0;
    level.is_complete = false;
       
    // create level maze, set level params and set up ghosts and players
    level.p_map = CreateLevelScene(game, level, player, ghosts);

    // start timer beggining with the chase mode
    level.chase_time_start = chrono::high_resolution_clock::now();

    // reset game
    game.SetGameOver(false);

}
void SpawnMonster(const Level& level, Ghost ghosts[])
{
    // spawn red ghost
    ghosts[0].ghost = Level::red_ghost;
    ghosts[0].square_content_now = ' ';
    ghosts[0].square_content_prior = ghosts[0].square_content_now;
    ghosts[0].current_pos = { ghosts[0].current_pos.row = 0, ghosts[0].current_pos.col = 0 };
    ghosts[0].spawn_pos = { ghosts[0].spawn_pos.row = 0, ghosts[0].spawn_pos.col = 0 };
    ghosts[0].previous_pos = { ghosts[0].previous_pos.row = 0, ghosts[0].previous_pos.col = 0 };
    ghosts[0].spawn_target = { ghosts[0].spawn_target.row = level.ghost_spawn.row, ghosts[0].spawn_target.col = level.ghost_spawn.row }; // just above spawn area
    ghosts[0].roam_target = { ghosts[0].roam_target.row = 24, ghosts[0].roam_target.col = 45 }; // roams to top right hand side
    ghosts[0].chase_modifier = { ghosts[0].chase_modifier.row = 0, ghosts[0].chase_modifier.col = 0 };
    ghosts[0].current_direction = Direction::UP;
    ghosts[0].previous_direction = Direction::UP;
    ghosts[0].mode = Mode::SPAWN;
    ghosts[0].run_first_move = true;
    ghosts[0].is_edible = false;
    ghosts[0].color_on = false;
    ghosts[0].color = 71;
    ghosts[0].wait = 15;
    ghosts[0].skip_turn = false; // use this to slow down moster if edible
    ghosts[0].look_ahead = 5; // how far ahead the IA looks for player

    // spawn yellow ghost
    ghosts[1].ghost = Level::yellow_ghost;
    ghosts[1].square_content_now = ' ';
    ghosts[1].square_content_prior = ghosts[1].square_content_now;
    ghosts[1].current_pos = { ghosts[1].current_pos.row = 0, ghosts[1].current_pos.col = 0 };
    ghosts[1].spawn_pos = { ghosts[1].spawn_pos.row = 0, ghosts[1].spawn_pos.col = 0 };
    ghosts[1].previous_pos = { ghosts[1].previous_pos.row = 0, ghosts[1].previous_pos.col = 0 };
    ghosts[1].spawn_target = { ghosts[0].spawn_target.row = level.ghost_spawn.row, ghosts[0].spawn_target.col = level.ghost_spawn.row }; // just above spawn area
    ghosts[1].roam_target = { ghosts[1].roam_target.row = 24, ghosts[1].roam_target.col = 2 }; // roams to top right hand side
    ghosts[1].chase_modifier = { ghosts[1].chase_modifier.row = 0, ghosts[1].chase_modifier.col = 3 };
    ghosts[1].current_direction = Direction::UP;
    ghosts[1].previous_direction = Direction::UP;
    ghosts[1].mode = Mode::SPAWN;
    ghosts[1].run_first_move = true;
    ghosts[1].is_edible = false;
    ghosts[1].color_on = false;
    ghosts[1].color = 367;
    ghosts[1].wait = 30;
    ghosts[1].skip_turn = false; // use this to slow down moster if edible
    ghosts[1].look_ahead = 5; // how far ahead the IA looks for player

    // spawn blue ghost
    ghosts[2].ghost = Level::blue_ghost;
    ghosts[2].square_content_now = ' ';
    ghosts[2].square_content_prior = ghosts[2].square_content_now;
    ghosts[2].current_pos = { ghosts[2].current_pos.row = 0, ghosts[2].current_pos.col = 0 };
    ghosts[2].spawn_pos = { ghosts[2].spawn_pos.row = 0, ghosts[2].spawn_pos.col = 0 };
    ghosts[2].previous_pos = { ghosts[2].previous_pos.row = 0, ghosts[2].previous_pos.col = 0 };
    ghosts[2].spawn_target = { ghosts[0].spawn_target.row = level.ghost_spawn.row, ghosts[0].spawn_target.col = level.ghost_spawn.row }; // just above spawn area
    ghosts[2].roam_target = { ghosts[2].roam_target.row = -3, ghosts[2].roam_target.col = 2 }; // roams to top right hand side
    ghosts[2].chase_modifier = { ghosts[2].chase_modifier.row = 0, ghosts[2].chase_modifier.col = -3 };
    ghosts[2].current_direction = Direction::UP;
    ghosts[2].previous_direction = Direction::UP;
    ghosts[2].mode = Mode::SPAWN;
    ghosts[2].run_first_move = true;
    ghosts[2].is_edible = false;
    ghosts[2].color_on = false;
    ghosts[2].color = 435;
    ghosts[2].wait = 45;
    ghosts[2].skip_turn = false; // use this to slow down moster if edible
    ghosts[2].look_ahead = 5; // how far ahead the IA looks for player

    // spawn pink ghost
    ghosts[3].ghost = Level::pink_ghost;
    ghosts[3].square_content_now = ' ';
    ghosts[3].square_content_prior = ghosts[3].square_content_now;
    ghosts[3].current_pos = { ghosts[3].current_pos.row = 0, ghosts[3].current_pos.col = 0 };
    ghosts[3].spawn_pos = { ghosts[3].spawn_pos.row = 0, ghosts[3].spawn_pos.col = 0 };
    ghosts[3].previous_pos = { ghosts[3].previous_pos.row = 0, ghosts[3].previous_pos.col = 0 };
    ghosts[3].spawn_target = { ghosts[0].spawn_target.row = level.ghost_spawn.row, ghosts[0].spawn_target.col = level.ghost_spawn.row }; // just above spawn area
    ghosts[3].roam_target = { ghosts[3].roam_target.row = -3, ghosts[3].roam_target.col = 45 }; // roams to top right hand side
    ghosts[3].chase_modifier = { ghosts[3].chase_modifier.row = -3, ghosts[3].chase_modifier.col = 0 };
    ghosts[3].current_direction = Direction::UP;
    ghosts[3].previous_direction = Direction::UP;
    ghosts[3].mode = Mode::SPAWN;
    ghosts[3].run_first_move = true;
    ghosts[3].is_edible = false;
    ghosts[3].color_on = false;
    ghosts[3].color = 479;
    ghosts[3].wait = 60;
    ghosts[3].skip_turn = false; // use this to slow down moster if edible
    ghosts[3].look_ahead = 5; // how far ahead the IA looks for player
}
void SpawnMonster(const Level& level, Ghost ghosts[], Ghost ghost, const bool playerDied)
{
    switch (ghost.ghost)
    {
    case Level::red_ghost:
        // spawn red ghost
        ghosts[0].square_content_now = ' ';
        ghosts[0].square_content_prior = ghosts[0].square_content_now;
        ghosts[0].current_pos = { ghosts[0].current_pos.row = ghosts[0].spawn_pos.row,  ghosts[0].current_pos.col = ghosts[0].spawn_pos.col };
        ghosts[0].previous_pos = { ghosts[0].previous_pos.row = ghosts[0].current_pos.row, ghosts[0].previous_pos.col = ghosts[0].current_pos.col };
        ghosts[0].current_direction = Direction::UP;
        ghosts[0].previous_direction = Direction::UP;
        ghosts[0].mode = Mode::SPAWN;
        ghosts[0].run_first_move = true;
        ghosts[0].is_edible = false;
        ghosts[0].color_on = false;
        ghosts[0].color = 71;
        ghosts[0].wait = (playerDied ? 15 : 15);
        ghosts[0].skip_turn = false; // use this to slow down moster if edible
        break;
    case Level::yellow_ghost:
        // spawn yellow ghost
        ghosts[1].square_content_now = ' ';
        ghosts[1].square_content_prior = ghosts[1].square_content_now;
        ghosts[1].current_pos = { ghosts[1].current_pos.row = ghosts[1].spawn_pos.row, ghosts[1].current_pos.col = ghosts[1].spawn_pos.col };
        ghosts[1].previous_pos = { ghosts[1].previous_pos.row = ghosts[1].current_pos.row, ghosts[1].previous_pos.col = ghosts[1].current_pos.col };
        ghosts[1].current_direction = Direction::UP;
        ghosts[1].previous_direction = Direction::UP;
        ghosts[1].mode = Mode::SPAWN;
        ghosts[1].run_first_move = true;
        ghosts[1].is_edible = false;
        ghosts[1].color_on = false;
        ghosts[1].color = 367;
        ghosts[1].wait = (playerDied ? 30 : 15);
        ghosts[1].skip_turn = false; // use this to slow down moster if edible
        break;
    case Level::blue_ghost:
        // spawn blue ghost
        ghosts[2].square_content_now = ' ';
        ghosts[2].square_content_prior = ghosts[2].square_content_now;
        ghosts[2].current_pos = { ghosts[2].current_pos.row = ghosts[2].spawn_pos.row, ghosts[2].current_pos.col = ghosts[2].spawn_pos.col };
        ghosts[2].previous_pos = { ghosts[2].previous_pos.row = ghosts[2].current_pos.row, ghosts[2].previous_pos.col = ghosts[2].current_pos.col };
        ghosts[2].current_direction = Direction::UP;
        ghosts[2].previous_direction = Direction::UP;
        ghosts[2].mode = Mode::SPAWN;
        ghosts[2].run_first_move = true;
        ghosts[2].is_edible = false;
        ghosts[2].color_on = false;
        ghosts[2].color = 435;
        ghosts[2].wait = (playerDied ? 45 : 15);
        ghosts[2].skip_turn = false; // use this to slow down moster if edible
        break;
    case Level::pink_ghost:
        // spawn pink ghost
        ghosts[3].square_content_now = ' ';
        ghosts[3].square_content_prior = ghosts[3].square_content_now;
        ghosts[3].current_pos = { ghosts[3].current_pos.row = ghosts[3].spawn_pos.row, ghosts[3].current_pos.col = ghosts[3].spawn_pos.col };
        ghosts[3].previous_pos = { ghosts[3].previous_pos.row = ghosts[3].current_pos.row, ghosts[3].previous_pos.col = ghosts[3].current_pos.col };
        ghosts[3].current_direction = Direction::UP;
        ghosts[3].previous_direction = Direction::UP;
        ghosts[3].mode = Mode::SPAWN;
        ghosts[3].run_first_move = true;
        ghosts[3].is_edible = false;
        ghosts[3].color_on = false;
        ghosts[3].color = 479;
        ghosts[3].wait = (playerDied ? 60 : 15);
        ghosts[3].skip_turn = false; // use this to slow down moster if edible
        break;
    }

    
}
char** CreateLevelScene(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    // load level info and map from scenes file using games current level
    string map = LoadSceneFromFile("PacMan.scenes", game.GetCurrentScene(), level);

    // if the string return "false" then you've reached the end of the game
    if (map == "false") {
        system("cls");
        cout << "You beat the PC! Good for you" << endl;
        system("pause");
        exit(0);
    }

    // parse through string to replace pellt and powerup markers to their ascii code
    ReplaceString(map, ".", char(Level::pellet));
    ReplaceString(map, "o", char(Level::powerup));

    // create dynamic two dimension pointer array to hold map
    Coord size = MapSize(map); // get width and height of the map
    level.cols = size.col; // set lsizes in level
    level.rows = size.row; // set sizes in level
      
    char** p_mapArray = new char*[size.row];
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
        if (map[i] ==(char)Level::pellet || map[i] ==(char)Level::powerup)
            level.total_pellets++;

        // Set player start to position 'S'
        if (map[i] == Level::player_start)
        {
            player.SetPositions(row, col);
        }

        // Set ghost spawn target location
        if (map[i] == Level::ghost_spawn_target)
        {
            level.ghost_spawn.row = row;
            level.ghost_spawn.col = col;
            p_mapArray[row][col] =(char)Level::pellet;
            level.total_pellets++;
            
            // set ghosts spawn target
            for (int g = 0; g < 4; g++) // loop through ghosts
            {
                ghosts[g].spawn_target.row = row;
                ghosts[g].spawn_target.col = col;
            }
        }

        // Set Ghost start position
        int ghostIndex = -1;
        switch (map[i])
        {
        case Level::red_ghost:
            ghostIndex = Level::gRED;
            break;
        case Level::yellow_ghost:
            ghostIndex = Level::gYELLOW;
            break;
        case Level::blue_ghost:
            ghostIndex = Level::gBLUE;
            break;
        case Level::pink_ghost:
            ghostIndex = Level::gPINK;
            break;
        }
        if (ghostIndex >= 0) {
            ghosts[ghostIndex].spawn_pos.row = ghosts[ghostIndex].current_pos.row = ghosts[ghostIndex].previous_pos.row = row;
            ghosts[ghostIndex].spawn_pos.col = ghosts[ghostIndex].current_pos.col = ghosts[ghostIndex].previous_pos.col = col;
        }
        

        // Set teleport coords for T1 and T2
        if (map[i] == Level::teleport) {
            if (row == 0 || col == 0) {
                level.tp_1 = Coord(row, col);
            }
            else {
                level.tp_2 = Coord(row, col);
            }
            // set teleport direction
            if (col == 0) {
                level.tp_row = true; // teleport across
            }
            else if (row == 0) {
                level.tp_row = false; // teleport certically
            }
        }
    }

    // return map array pointer
    return p_mapArray;
}
void DrawLevel(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    Draw draw;
    
    // set the content of the sqaure the player is moving into
    char plyayer_move_content = level.p_map[player.GetCurrentRow()][player.GetCurrentCol()];
        
    int current_pellets = 0; // bug fix - loosing some pellets on map - will need to perma fix but this will do for now
    
    // set the content of the sqaure the ghosts are currently back to what they moved over (pellet, power up or space)
    for (int r = 0; r < level.rows; r++)
    {
        for (int c = 0; c < level.cols; c++)
        {
            switch(level.p_map[r][c])
            {
            case Level::red_ghost:
                level.p_map[r][c] = ghosts[level.gRED].square_content_prior;
            case Level::yellow_ghost:
                level.p_map[r][c] = ghosts[level.gYELLOW].square_content_prior;
            case Level::blue_ghost:
                level.p_map[r][c] = ghosts[level.gBLUE].square_content_prior;
            case Level::pink_ghost:
                level.p_map[r][c] = ghosts[level.gPINK].square_content_prior;
            }
        }
    }
    
    // place cursor on top left of console
    draw.CursorTopLeft(level.rows + 5); // + 5 for title and status

    // remove player from map at last position if they are different
    if (!player.GetPreviousPosition().IsSame(player.GetCurrentPosition()))
        level.p_map[player.GetPreviousRow()][player.GetPreviousCol()] = Level::space;

    // player in tunnel
    if (player.GetCurrentPosition().IsSame(level.tp_1)) {
        level.p_map[player.GetCurrentRow()][player.GetCurrentCol()] = Level::teleport;
        player.SetCurrentPosition(level.tp_2);
    }
    else if (player.GetCurrentPosition().IsSame(level.tp_2)) {
        level.p_map[player.GetCurrentRow()][player.GetCurrentCol()] = Level::teleport;
        player.SetCurrentPosition(level.tp_1);
    }
    
    for (int g = 0; g < 4; g++) // loop through ghots
    {
        // remove ghosts from map at last position if they are different
        if (ghosts[g].previous_pos.row != ghosts[g].current_pos.row || ghosts[g].previous_pos.col != ghosts[g].current_pos.col)
            level.p_map[ghosts[g].previous_pos.row][ghosts[g].previous_pos.col] = Level::space;
        
        // ghost in tunnel
        if (!ghosts[g].skip_turn)
        {
            if (ghosts[g].current_pos.row == level.tp_1.row && ghosts[g].current_pos.col == level.tp_1.col) {
                level.p_map[ghosts[g].current_pos.row][ghosts[g].current_pos.col] = Level::teleport;
                ghosts[g].current_pos.col = level.tp_2.col;
            }
            else if (ghosts[g].current_pos.row == level.tp_2.row && ghosts[g].current_pos.col == level.tp_2.col) {
                level.p_map[ghosts[g].current_pos.row][ghosts[g].current_pos.col] = Level::teleport;
                ghosts[g].current_pos.col = level.tp_1.col;
            }
        }
        
    }
   
    // Level Title
    draw.SetColor(Level::cWHITE);
    cout << endl << "                 PACMAN: " << TransformString(level.title, 0) << endl << endl;

    // draw current level map
    for (int r = 0; r < level.rows; r++)
    {
        for (int c = 0; c < level.cols; c++)
        {
            // position player
            if (player.GetCurrentPosition().IsSame(Coord(r,c)))
                level.p_map[r][c] = player.character;

            for (int g = 0; g < 4; g++) // loop through ghots
            {
                // position ghost
                if (r == ghosts[g].current_pos.row && c == ghosts[g].current_pos.col)
                    level.p_map[r][c] = ghosts[g].ghost;

                // if the ghost moved from the last position
                if (ghosts[g].previous_pos.row != ghosts[g].current_pos.row || ghosts[g].previous_pos.col != ghosts[g].current_pos.col)
                {
                    // put back the content of the square the ghost was last at 
                    if (r == ghosts[g].previous_pos.row && c == ghosts[g].previous_pos.col)
                        level.p_map[r][c] = ghosts[g].square_content_prior;
                }
            }

            // set color of map content at this position
            switch (level.p_map[r][c])
            {
            case Level::invisible_wall: // % = invisible wall
            case Level::teleport: // T = portal
            case Level::space: // ' ' = empty space
            case Level::one_way: // $ = one way door for ghost spawn area
                draw.SetColor(Level::cINVISIBLE); // black on black = not visible
                break;
            case (char)Level::pellet: // . = pellet
                current_pellets++;
                draw.SetColor(Level::cWHITE); // gray for bullets
                break;
            case (char)Level::powerup: // o = power up
                current_pellets++;
                draw.SetColor(Level::cWHITE); // white for power ups
                break;
            case Level::pink_ghost: // blue ghost
                if (ghosts[Level::gPINK].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[Level::gPINK].color_on ? draw.SetColor(Level::cGHOST_ON) : draw.SetColor(Level::cGHOST_OFF);
                    ghosts[Level::gPINK].color_on = !ghosts[Level::gPINK].color_on;
                }
                else { // solid color
                    draw.SetColor(ghosts[Level::gPINK].color);
                }
                break;
            case Level::yellow_ghost: // yellow ghost
                if (ghosts[Level::gYELLOW].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[Level::gYELLOW].color_on ? draw.SetColor(Level::cGHOST_ON) : draw.SetColor(Level::cGHOST_OFF);
                    ghosts[Level::gYELLOW].color_on = !ghosts[Level::gYELLOW].color_on;
                } else { // solid color
                    draw.SetColor(ghosts[Level::gYELLOW].color);
                }
                break;
            case Level::blue_ghost: // green ghost
                if (ghosts[Level::gBLUE].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[Level::gBLUE].color_on ? draw.SetColor(Level::cGHOST_ON) : draw.SetColor(Level::cGHOST_OFF);
                    ghosts[Level::gBLUE].color_on = !ghosts[Level::gBLUE].color_on;
                }
                else { // solid color
                    draw.SetColor(ghosts[Level::gBLUE].color);
                }
                break;
            case Level::red_ghost: // red ghost
                if (ghosts[Level::gRED].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[Level::gRED].color_on ? draw.SetColor(Level::cGHOST_ON) : draw.SetColor(Level::cGHOST_OFF);
                    ghosts[Level::gRED].color_on = !ghosts[Level::gRED].color_on;
                } else { // solid color
                    draw.SetColor(ghosts[Level::gRED].color);
                }
                break;
            case player.character: // player
                draw.SetColor(Level::cPLAYER);
                break;
            default:
                draw.SetColor(Level::cWALLS); // gray bg on gray text for all other chars making them walls essentially
                break;
            }

            // print char
            cout << level.p_map[r][c];

            // set color back to default
            draw.SetColor(Level::cWHITE);
        }
        // end of row ad line feed
        cout << endl;
    }
        
    if (level.level_paused) {
        // before start of level get any key to start
        cout << endl;
        cout << "           Press any key to start.             ";
        SFX(game, level, Play::INTERMISSION);
        char input = _getch();
        level.level_paused = false;
    }
    else
    {
        // play the appropriate game sound
        switch (plyayer_move_content)
        {
        case (char)Level::pellet:
            SFX(game, level, Play::MUNCH);
            break;
        case (char)Level::powerup:
            SFX(game, level, Play::POWER_UP);
            break;
        case Level::space:
        case player.character:
            SFX(game, level, Play::SIREN);
            break;
        }
    }

    // ********
    // BUG FIX - brute force - need to look at why we are dropping a few pellets
    level.eaten_pellets = level.eaten_pellets + (level.total_pellets - level.eaten_pellets - current_pellets);

}
void StatusBar(Game& game, Level& level, Player& player)
{
    Draw draw;
    // get number of lives
    string lives;
    for (int i = 0; i < player.Lives(); i++)
    {
        lives = lives + player.character;
    }
    player.SetScore(((level.eaten_pellets + 1) * level.points_pellet) + (level.eaten_ghosts * level.points_ghost) + (level.eaten_ghosts >= 4 ? level.all_ghost_bonus : 0));
    cout << endl;
    draw.SetColor(7);
    cout << "      LIVES:";
    draw.SetColor(14);
    cout << (player.Lives() > 0 ? lives : "-");
    draw.SetColor(7);
    cout << "   SCORE:";
    draw.SetColor(14);
    cout << setfill('0') << setw(8) << player.GetScore();
    draw.SetColor(7);
    cout << "         ";
    cout << "\r";

    // message game over or level complete
    if (game.IsGameOver())
    {
        cout << "      Game OVer! Press a key to continue.      ";
    }
    else if (level.is_complete)
    {
        cout << "    Level complete! Press a key to continue.    ";
    }
       
}
int SFX(Game& game, Level& level, Play playSFX)
{
    LPCTSTR sfx = NULL;
    bool played = false;
    bool timerOn = false;
    DWORD params = SND_FILENAME | SND_ASYNC;
    float duration = 0.0f;

    switch (playSFX)
    {
    case Play::INTRO:
        sfx = TEXT("sfx_intro.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::INTERMISSION:
        sfx = TEXT("sfx_intermission.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::MUNCH:
        if (game.GetPlaySFX() == Play::MUNCH)
            return 0;
        sfx = TEXT("sfx_munch_lg.wav");
        params = SND_LOOP | SND_FILENAME | SND_ASYNC;
        break;
    case Play::DEATH:
        sfx = TEXT("sfx_death.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::CREDIT:
        sfx = TEXT("sfx_bonus.wav");
        break;
    case Play::POWER_UP:
        sfx = TEXT("sfx_powerup.wav");
        params = SND_LOOP | SND_FILENAME | SND_ASYNC;
        break;
    case Play::SIREN:
        if (game.GetPlaySFX() == Play::SIREN)
            return 0;
        sfx = TEXT("sfx_siren.wav");
        params = SND_LOOP | SND_FILENAME | SND_ASYNC;
        break;
    case Play::LIFE:
        sfx = TEXT("sfx_xlife.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::EAT_GHOST:
        sfx = TEXT("sfx_eatghost.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::EAT_FRUIT:
        sfx = TEXT("sfx_eatfruit.wav");
        break;
    default:
        sfx = NULL;
        break;
    }

    // immediately cancel all audio sfx is trying to play none
    if (playSFX == Play::NONE)
    {
        played = PlaySound(NULL, NULL, params);
        game.SetPlaySFX(Play::NONE);
        return 0;
    }

    // if mode is RUN and not ghost eat or death, then play power up
    if (level.level_mode == Mode::RUN)
    {
        if (game.GetPlaySFX() != Play::POWER_UP)
        {
            played = PlaySound(TEXT("sfx_powerup.wav"), NULL, SND_LOOP | SND_FILENAME | SND_ASYNC);
            game.SetPlaySFX(Play::POWER_UP);
            return 0;
        }
    }

    if (playSFX != game.GetPlaySFX())
    {
        if (playSFX == Play::DEATH || playSFX == Play::EAT_GHOST || playSFX == Play::LIFE)
        {
            played = PlaySound(sfx, NULL, params);
            game.SetPlaySFX(playSFX);
            return 0;
        }
        else if (level.level_mode != Mode::RUN)
        {
            played = PlaySound(sfx, NULL, params);
            game.SetPlaySFX(playSFX);
            return 0;
        }
    }

    return 0;

}
string LoadSceneFromFile(string filename, int scene, Level& level)
{
    ifstream scenesFile(filename);
    string fileLine, section, map = "";
    bool processLines = true;
    
    if (scenesFile) {
        // file exists and is open 
        while(getline(scenesFile, fileLine))
        {
            
            //check for right scene
            section = "#scene:";
            if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) { // if line is scene identifier

                if (scene == stoi(fileLine.substr(TransformString(section, 1).size(), 2))) // and if the sence is the one I'm looking for
                {
                    //this is the scene we are looking for so let's process it until your get to the end of the level info
                    while(getline(scenesFile, fileLine))
                    {
                        // get the next line and set the relevant info in the level info

                        section = "title:";
                        if (fileLine.find(TransformString(section,1), 0) != std::string::npos) {
                            level.title = fileLine.substr(TransformString(section, 1).size(), (fileLine.size()-section.size()));
                            continue;
                        }

                        section = "pellet_points:";
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
                            level.points_ghost = stoi(fileLine.substr(TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "ghost_points:";
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
                            level.points_ghost = stoi(fileLine.substr(TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "all_ghosts_bonus:";
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
                            level.all_ghost_bonus = stoi(fileLine.substr(TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "edible_ghost duration:";
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
                            level.edible_ghost_duration = stoi(fileLine.substr(TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "chase_duration:";
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
                            level.chase_for = stoi(fileLine.substr(TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "run_duration:";
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
                            level.run_for = stoi(fileLine.substr(TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "roam_duration:";
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
                            level.roam_for = stoi(fileLine.substr(TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "roam_count:";
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
                            level.roam_count = stoi(fileLine.substr(TransformString(section, 1).size(), (fileLine.size() - section.size())));
                            continue;
                        }

                        section = "level_map:";
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
                            // get map info into the map string ending when you reach end of map
                            section = "#end_scene";
                            while (getline(scenesFile, fileLine))
                            {
                                if (fileLine.find(TransformString(section, 1), 0) == std::string::npos)
                                    map += fileLine;
                                else
                                    break;
                            }
                        }

                        section = "#scene:"; // check to see if reached the next scene;
                        if (fileLine.find(TransformString(section, 1), 0) != std::string::npos) {
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
#pragma endregion

#pragma region Player movement
void GetPlayerDirection(Game& game)
{
    game.GetKeyboardInput();
    //int input;

    //do
    //{
    //    input = _getch();
    //    
    //    // if special character then get special key into input
    //    (input && input == 224) ? input = _getch() : input;

    //     switch (input)
    //    {
    //    case Game::kARROW_LEFT:
    //    case Game::kLEFT:
    //        player.SetDirection(Direction::LEFT);
    //        break;
    //    case Game::kARROW_RIGHT:
    //    case Game::kRIGHT:
    //        player.SetDirection(Direction::RIGHT);
    //        break;
    //    case Game::kARROW_UP:
    //    case Game::kUP:
    //        player.SetDirection(Direction::UP);
    //        break;
    //    case Game::kARROW_DOWN:
    //    case Game::kDOWN:
    //        player.SetDirection(Direction::DOWN);
    //        break;
    //    default:
    //        break;
    //    }
    //} while (!level.is_complete && !game.IsGameOver());
}
void MovePlayer(Level& level, Player& player, Ghost ghosts[])
{
    player.SetPreviousPosition(player.GetCurrentPosition());
    
    Coord currentPos = player.GetCurrentPosition();
    Coord nextPos(currentPos, player.GetPreviousDirection());

    switch (player.GetDirection())
    {
    case Direction::UP: //up
        if (NotWall(level, player, Coord(currentPos, Direction::UP), Direction::UP)) {
            nextPos.SetTo(currentPos, Direction::UP);
            player.SetPreviousDirection(Direction::UP);
        }
        break;
    case Direction::RIGHT: // right
        if (NotWall(level, player, Coord(currentPos, Direction::RIGHT), Direction::RIGHT)) {
            nextPos.SetTo(currentPos, Direction::RIGHT);
            player.SetPreviousDirection(Direction::RIGHT);
        }
        break;
    case Direction::DOWN: // down
        if (NotWall(level, player, Coord(currentPos, Direction::DOWN), Direction::DOWN)) {
            nextPos.SetTo(currentPos, Direction::DOWN);
            player.SetPreviousDirection(Direction::DOWN);
        }
        break;
    case Direction::LEFT: // left
        if (NotWall(level, player, Coord(currentPos, Direction::LEFT), Direction::LEFT)) {
            nextPos.SetTo(currentPos, Direction::LEFT);
            player.SetPreviousDirection(Direction::LEFT);
        }
        break;
    }

    if (!NotWall(level, player, nextPos, player.GetPreviousDirection())) {
            nextPos = currentPos;
    }

    // move the player
    player.SetCurrentPosition(nextPos);

    // set any statuses that need setting
    SetPlayerState(level, player, ghosts);

    // check collision with a ghost and set the ghosts previous row to the new player position
    for (int g = 0; g < 4; g++)
    {
        if (CheckCollision(player, ghosts[g])) {
            ghosts[g].previous_pos.SetTo(player.GetCurrentPosition());
        }
    }

}
#pragma endregion

#pragma region  Ghost movement
int MoveGhost(Game& game, Level& level, Player& player, Ghost& ghost, Ghost ghosts[])
{
    Direction bestMove = Direction::NONE; // will store the next move

    for (int g = 0; g < 4; g++)
    {
        if (ghosts[g].skip_turn || game.IsGameOver() || CheckCollision(player, ghosts[g]))
        {
            continue; // next ghost (no move)
        }

        if (ghosts[g].wait > 0)
        {
            ghosts[g].wait--; // ghost has respawned and we are counting dowm the tics to let it move
            continue; // next ghost (no move)
        }

        if (ghosts[g].mode == Mode::RUN) // make random moves truning opposite direction on first move
        {
            bestMove = RandomGhostMove(level, player, ghosts[g]);
            DoGhostMove(level, player, ghosts, ghosts[g], bestMove);
            continue; // next ghost
        }

        if (ghosts[g].mode != Mode::RUN)  // run recursive AI for chase, roam and spawn modes 
        {
            int score = 0, bestScore = 1000;
            Coord nextMove;
            Coord move(ghosts[g].current_pos.row, ghosts[g].current_pos.col);
            Direction new_direction = Direction::NONE;

            for (int i = 0; i <= 3; i++) // cycle through up,down,left,right to find the valid best next move
            {
                new_direction = static_cast<Direction>(i);
                if (NotWall(level, player, Coord(move, new_direction), new_direction) && !IsReverse(ghosts[g].current_direction, new_direction)) // check for next available square
                {
                    // set new coords of ghost
                    nextMove.SetTo(move, new_direction);
                    ghosts[g].current_pos.SetTo(nextMove);

                    // calculate distance score based on new coords
                    int score = GetBestMove(level, player, ghosts[g], nextMove, new_direction, 1, true); // get the minimax score for the move (recurssive)

                    // revert ghost coords back
                    ghosts[g].current_pos.SetTo(move);

                    // if the new move score gets the ghost closer to the target coord
                    if (score < bestScore)
                    {
                        bestScore = score; // set new best score
                        bestMove = new_direction; // set new best move direction
                    }

                }
            }
            DoGhostMove(level, player, ghosts, ghosts[g], bestMove);
            continue; // next ghost
        }
    }

    return 0; // clean exit
}
Direction RandomGhostMove(const Level& level, Player& player, Ghost& ghost)
{
    Direction newDirection = Direction::NONE;

    // first move when on the run is ALWAYS to reverse direction which is ALWAYS a valid move
    if (ghost.run_first_move) {
        switch (ghost.current_direction)
        {
        case Direction::UP:
            newDirection = Direction::DOWN;
            break;
        case Direction::RIGHT:
            newDirection = Direction::LEFT;
            break;
        case Direction::DOWN:
            newDirection = Direction::UP;
            break;
        case Direction::LEFT:
            newDirection = Direction::RIGHT;
            break;
        }
        ghost.run_first_move = false;
        return newDirection;
    }

    // if this is not the first move chose the next move randomly
    Coord move(ghost.current_pos.row, ghost.current_pos.col);
    int unsigned seed = (int)(std::chrono::system_clock::now().time_since_epoch().count());
    srand(seed);

    if (IsTeleport(level, move)) // if on teleportkeep the same direction
        ghost.current_direction == Direction::LEFT ? newDirection = Direction::LEFT : newDirection = Direction::RIGHT;
    else // else choose a valid random direction
    {
        do
        {
            int randomNumber = rand() % 4; //generate random number to select from the 4 possible options
            newDirection = static_cast<Direction>(randomNumber);
        } while (!NotWall(level, player, Coord(move, newDirection), newDirection) || IsReverse(ghost.current_direction, newDirection));
    }

    return newDirection;

}
int GetBestMove(Level& level, Player& player, Ghost& ghost, Coord move, Direction curr_direction, int depth, bool isGhost)
{
    // and on the target the ghost chases: red chases player pos, yellow player pos + 2 cols (to the right of player)
    switch (ghost.mode)
    {
    case Mode::CHASE: // redude distance to player
        if (ghost.current_pos.DistanceTo(player.GetCurrentPosition(), ghost.chase_modifier) == 0) { // if player and ghost collide return 0 + depth as score
            return 0 + depth; // add depth to get fastest path
        }
        if (depth == ghost.look_ahead) { // if depth X return the distance score, increase this to make the ghost look forward more
            return (ghost.current_pos.DistanceTo(player.GetCurrentPosition(), ghost.chase_modifier) + depth); // add depth to get fastest path
        }
        break;
    case Mode::ROAM: // reduce distance to the ghost's roam target
        if (depth == ghost.look_ahead) {
            return abs(ghost.current_pos.DistanceTo(ghost.roam_target));
        }
        break;
    case Mode::SPAWN: // target is above the exit area
        int distance_to_targt = abs(ghost.current_pos.DistanceTo(ghost.spawn_target));
        if (distance_to_targt == 0)
            ghost.mode = (level.level_mode == Mode::RUN ? Mode::CHASE : level.level_mode);
        return distance_to_targt;
        break;
    }

    if (isGhost) {
        // if its the ghost move we want the lowest possible distance to player
        int score = 1000, bestScore = ghost.mode == Mode::RUN ? -1000 : 1000;
        Coord nextMove;
        Direction new_direction = Direction::NONE;

        for (int i = 0; i <= 3; i++) // cycle through each next possible move up, down, left, right
        {
            new_direction = static_cast<Direction>(i); // cast index to direction up, down, left, right
            if (NotWall(level, player, Coord(move, new_direction), new_direction) && !IsReverse(curr_direction, new_direction)) // check if the direction is valid i.e not wall or reverse
            {
                // set new coords of ghost
                nextMove.SetTo(move, new_direction);
                ghost.current_pos.SetTo(nextMove);

                // calculate distance score based on new coords
                int score = GetBestMove(level, player, ghost, nextMove, new_direction, depth + 1, true); // get the minimax score for the move (recurssive)

                // revert ghost coords back
                ghost.current_pos.SetTo(move);

                // if the score of is better than the current bestscore set the score to be the new best score (min to get closer, max when running away)
                if (ghost.mode == Mode::RUN)
                    bestScore = max(score, bestScore);
                else
                    bestScore = min(score, bestScore);
            }
        }
        return bestScore;
    }
    else
    {
        // can add player move logic here if we want to take into account player moving optimally
        return 0;
    }

}
Coord GhostTeleport(Level& level, Ghost& ghost, Coord move, Direction direction)
{
    if (level.tp_1.row == move.row && level.tp_1.col == move.col && ghost.previous_pos.col != level.tp_1.col)
        if(direction == Direction::LEFT)
            return { level.tp_2.row, level.tp_2.col };
        else
            return { move.row, move.col + 1 };

    if (level.tp_2.row == move.row && level.tp_2.col == move.col && ghost.previous_pos.col != level.tp_2.col)
        if (direction == Direction::RIGHT)
            return { level.tp_1.row, level.tp_1.col };
        else
            return { move.row, move.col - 1 };
    return move;
}
void DoGhostMove(Level& level, Player& player, Ghost ghosts[], Ghost& ghost, Direction direction)
{
    ghost.previous_pos.row = ghost.current_pos.row;
    ghost.previous_pos.col = ghost.current_pos.col;
    ghost.square_content_prior = ghost.square_content_now;

    switch (direction)
    {
    case Direction::UP: //up
        ghost.square_content_now = GetSquareContentNow(level, ghosts, { ghost.current_pos.row -1, ghost.current_pos.col});
        ghost.current_pos.row--;
        ghost.current_direction = Direction::UP;
        break;
    case Direction::RIGHT: // right
        ghost.square_content_now = GetSquareContentNow(level, ghosts, { ghost.current_pos.row, ghost.current_pos.col + 1 });
        ghost.current_pos.col++;
        ghost.current_direction = Direction::RIGHT;
        break;
    case Direction::DOWN: // down
        ghost.square_content_now = GetSquareContentNow(level, ghosts, { ghost.current_pos.row + 1, ghost.current_pos.col});
        ghost.current_pos.row++;
        ghost.current_direction = Direction::DOWN;
        break;
    case Direction::LEFT: // left
        ghost.square_content_now = GetSquareContentNow(level, ghosts, { ghost.current_pos.row, ghost.current_pos.col - 1 });
        ghost.current_pos.col--;
        ghost.current_direction = Direction::LEFT;
        break;
    default:
        break;
    }
    // if the sqaure the ghost moved into is the player
    ghost.square_content_now == player.character ? ghost.square_content_now = Level::space : ghost.square_content_now;

    // if the mosnter is over the player save a blank space to buffer
    CheckCollision(player, ghost) ? ghost.square_content_now = Level::space : ghost.square_content_now;

}
char GetSquareContentNow(Level& level, Ghost ghosts[], Coord square)
{
    switch (level.p_map[square.row][square.col])
    {
        case (char)Level::pellet:
        case (char)Level::powerup:
        case ' ':
            return(level.p_map[square.row][square.col]);
        case Level::red_ghost:
            return ghosts[0].square_content_now;
        case Level::yellow_ghost:
            return ghosts[1].square_content_now;
        case Level::blue_ghost:
            return ghosts[2].square_content_now;
        case Level::pink_ghost:
            return ghosts[3].square_content_now;
        default:
            return(level.p_map[square.row][square.col]);
    }
}
#pragma endregion

#pragma region Player & Ghost Common Movement functions
bool NotWall(const Level& level, const Player& player, const Coord& move, const Direction& direction)
{
    switch (level.p_map[move.row][move.col])
    {
    case Level::space:
    case Level::teleport:
    case (char)Level::pellet:
    case (char)Level::powerup:
    case Level::red_ghost:
    case Level::yellow_ghost:
    case Level::blue_ghost:
    case Level::pink_ghost:
    case player.character:
        return true;
        break;
    case Level::one_way: // one way move to exit ghost spawn area
        return (direction != Direction::DOWN ? true : false);
        break;
    }
    return false;
}
bool IsTeleport(const Level& level, const Coord& move)
{
    if (level.tp_1.row == move.row && level.tp_1.col == move.col)
        return true;
    if (level.tp_2.row == move.row && level.tp_2.col == move.col)
        return true;
    return false;
}
bool IsReverse(const Direction& curr_direction, const Direction& new_direction)
{
    switch (new_direction)
    {
    case Direction::UP:
        return (curr_direction == Direction::DOWN ? true : false);
        break;
    case Direction::RIGHT:
        return (curr_direction == Direction::LEFT ? true : false);
        break;
    case Direction::DOWN:
        return (curr_direction == Direction::UP ? true : false);
        break;
    case Direction::LEFT:
        return (curr_direction == Direction::RIGHT ? true : false);
        break;
    }
    return false;
}
#pragma endregion

#pragma region Player/Ghost Events and Status
void SetPlayerState(Level& level, Player& player, Ghost ghosts[])
{
    char levelObj = level.p_map[player.GetCurrentRow()][player.GetCurrentCol()];
    switch (levelObj)
    {
    case (char)Level::pellet: // eat pellet
        level.eaten_pellets++;
        break;
    case (char)Level::powerup: // eat power up
        for (int g = 0; g < 4; g++) // loop ghosts
        {
            ghosts[g].mode != Mode::SPAWN ? ghosts[g].is_edible = true : ghosts[g].is_edible = false; // ghost only edible on power up if out of spwan area
            ghosts[g].is_edible ? ghosts[g].mode = Mode::RUN : ghosts[g].mode = ghosts[g].mode;
            ghosts[g].mode == Mode::RUN ? ghosts[g].run_first_move = true : ghosts[g].run_first_move = false;
        }
        if (level.level_mode != Mode::RUN)
            player.ClearEatenGohsts();
        level.level_mode = Mode::RUN;
        level.run_time_start = chrono::high_resolution_clock::now();
        level.eaten_pellets++;
        break;
    }
}
void SetGhostMode(Level& level, Player& player, Ghost ghosts[])
{
    switch (level.level_mode)
    {
   
    // set chase mode stuff
    case Mode::CHASE:
        level.chase_duration = duration_cast<duration<double>>(level.chase_time_start - chrono::high_resolution_clock::now());
        if (abs(level.chase_duration.count()) > level.chase_for && level.roam_count > 0)
        {
            level.level_mode = Mode::ROAM;
            level.roam_time_start = chrono::high_resolution_clock::now();
            level.roam_count--;

            for (int g = 0; g < 4; g++) {
                switch (ghosts[g].mode)
                {
                case Mode::ROAM:
                case Mode::CHASE:
                    ghosts[g].mode = Mode::ROAM;
                    ghosts[g].skip_turn = false;
                    break;
                case Mode::RUN:
                case Mode::SPAWN:
                    ghosts[g].mode = ghosts[g].mode;
                    break;
                }
            }
        }
        break;
    
    // set roam mode stuff
    case Mode::ROAM:
        level.roam_duration = duration_cast<duration<double>>(level.roam_time_start - chrono::high_resolution_clock::now());

        if (abs(level.roam_duration.count()) > level.roam_for)
        {
            level.level_mode = Mode::CHASE;
            level.chase_time_start = chrono::high_resolution_clock::now();

            for (int g = 0; g < 4; g++) {
                switch (ghosts[g].mode)
                {
                case Mode::ROAM:
                    ghosts[g].mode = Mode::CHASE;
                    break;
                case Mode::CHASE:
                case Mode::RUN:
                case Mode::SPAWN:
                    ghosts[g].mode = ghosts[g].mode;
                }
            }
        }
        break;
    
    // set run mode stuff
    case Mode::SPAWN:
    for (int g = 0; g < 4; g++)
    {
        if (ghosts[g].current_pos.row == level.ghost_spawn.row && ghosts[g].current_pos.col == level.ghost_spawn.col)
        {
            ghosts[g].mode = Mode::CHASE;
        }
    }
    break;

    // set run mode stuff
    case Mode::RUN:
        level.run_duration = duration_cast<duration<double>>(level.run_time_start - chrono::high_resolution_clock::now());
        if (abs(level.run_duration.count()) > level.run_for) // end the run mode
        {
            level.level_mode = Mode::CHASE;
            level.chase_time_start = chrono::high_resolution_clock::now();

            for (int g = 0; g < 4; g++) {
                switch (ghosts[g].mode)
                {
                case Mode::RUN:
                case Mode::ROAM:
                    ghosts[g].mode = Mode::CHASE;
                    break;
                case Mode::CHASE:
                case Mode::SPAWN:
                    ghosts[g].mode = ghosts[g].mode;
                }
                ghosts[g].skip_turn = false; // when ghost is edible slow him down
                ghosts[g].is_edible = false;
                ghosts[g].run_first_move = true;
            }
        }
        else // set the ghosts states while in run mode - some ghots can be in run mode and others in chase mode
        {
            for (int g = 0; g < 4; g++) {
                if (ghosts[g].mode == Mode::RUN)
                    ghosts[g].skip_turn = !ghosts[g].skip_turn; // when ghost is edible slow him down
                else
                    ghosts[g].skip_turn = false;
            }

        }
        break;
    }


}
void PlayerMonsterCollision(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    bool player_died = false;
    
    for (int g = 0; g < 4; g++) // loop ghosts
    {
        if (CheckCollision(player, ghosts[g]))
        {
            DrawLevel(game, level, player, ghosts); // print the move immediately
            if (ghosts[g].is_edible) // ghost dies
            {
                // if the ghost was on a pellet/powerup sqaure need to count it up
                if (ghosts[g].square_content_now ==(char)Level::pellet || ghosts[g].square_content_now == (char)Level::powerup)
                    level.eaten_pellets++;
                level.eaten_ghosts++; // increment ghosts eaten
                game.SetPauseForGobble(true); // set small pause after eating ghost
                SpawnMonster(level, ghosts, ghosts[g], false); // reset ghost to spawn area whihc resets mode to spawn
                player.EatGhost(g);
                if  (player.AllGhostsEaten()) {
                    level.all_eaten_ghosts += 1;
                    SFX(game, level, Play::LIFE);
                    player.ClearEatenGohsts();
                    level.level_mode = Mode::CHASE;
                    level.chase_time_start = chrono::high_resolution_clock::now();
                }
                else {
                    SFX(game, level, Play::EAT_GHOST);
                }
                
            }
            else
            {
                // player dies
                player_died = true;
            }
        }
    }

    if (player_died) {
        
        // rest the roam count by adding one back to the count
        level.level_mode == Mode::ROAM ? level.roam_count++ : level.roam_count;

        // set the level mode to chase
        level.level_mode = Mode::CHASE;
        level.chase_time_start = chrono::high_resolution_clock::now();

        // end game or respawn player if no lives left
        player.TakeLives(1);
        if (!player.HasNoLives())
        {
            game.SetPauseForPlayerDeath(true); // give player time to get ready
            player.ReSpawn();
            for (int g = 0; g < 4; g++) {
                // replace ghost with empty sqaure since the player was there an any pellet got eaten
                level.p_map[ghosts[g].current_pos.row][ghosts[g].current_pos.col] = ' ';
                // respawn ghost
                SpawnMonster(level, ghosts, ghosts[g], true);
            }
            SFX(game, level, Play::DEATH);
        }
        else
        {
            game.SetGameOver(true);
            SFX(game, level, Play::DEATH);
        }
    }

}
bool CheckCollision(Player& player, Ghost& ghost)
{
    return(player.GetCurrentPosition().IsSame(ghost.current_pos) ? true : false);
}
#pragma endregion

#pragma region Game management
void RefreshDelay(Game& game, Level& level)
{
    if (!level.is_complete && !game.IsGameOver())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(Game::refresh_delay)); // pause to slow game
    }
}
void PlayerMonsterCollisionDelay(Game& game, Level& level)
{
    if (game.PauseForGobble())
    {
        game.SetPauseForGobble(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(Game::gobble_delay)); // pause for ghost gobble
    }
    else if (game.PauseForPalyerDeath())
    {
        game.SetPauseForPlayerDeath(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(Game::player_beat_delay)); // pause for player gobble
    }
}
void CheckLevelComplete(Level& level)
{
    level.eaten_pellets >= level.total_pellets ? level.is_complete = true : level.is_complete = false; // if all pellets are eaten game ends - 339 in this map
}
bool NextLevelRestartGame(Game& game, Level& level)
{
    if (game.IsGameOver())
    {
        
        cout << "\r";
        cout << "       play again? 'y' = yes, 'n' = no         ";
        char input = _getch();
        if (input == Game::kNO)
        {
            DeallocateMem(level);
            return false;
        }
        else
        {
            DeallocateMem(level);
            system("cls");
            game.SetCurrentScene(1);
            return true;
        }
    }
    DeallocateMem(level);
    game.NextScene();
    return true;
}
void DeallocateMem(Level& level)
{
    // dealocate dynamic array with level map
    for (int i = 0; i < level.rows; i++) {
        delete[] level.p_map[i];
    }
    delete[] level.p_map;
    level.p_map = nullptr;
}
#pragma endregion

#pragma region Helper and scaffolding
string TransformString(string text, int operation)
{
    string temp = text;
    switch (operation)
    {
    case 0: // to upper
        transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
        return temp;
    case 1: // to lower
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        return temp;
    }
    return "";
}
string GhostMode(Ghost& ghost)
{
    switch (ghost.mode)
    {
    case Mode::CHASE:
        return "CHASE";
    case Mode::ROAM:
        return "ROAM";
    case Mode::RUN:
        return "RUN";
    case Mode::SPAWN:
        return "SPAWN";
    default:
        return "NONE";
    }
    return "NONE";
}
void ReplaceString(string& text, const string from, const char to)
{
    size_t start_pos = 0;
    string to_string = { to };
    while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
    text.replace(start_pos, from.length(), to_string);
    start_pos += to_string.length(); // ...
    }
}
Coord MapSize(const string& map)
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
    return{ Coord(int(rows),(int)cols) };
}
#pragma endregion

#pragma region info, about, credits
void Credits(Game& game, Level& level)
{
    Draw draw;
    string ghost;

    ghost += "               *********\n";
    ghost += "           *****************\n";
    ghost += "         *********************\n";
    ghost += "       ******####********####**\n";
    ghost += "       ****#########***#########\n";
    ghost += "       ****#####    ***#####    *\n";
    ghost += "    *******#####    ***#####    ***\n";
    ghost += "    *********#####********####*****\n";
    ghost += "    *******************************\n";
    ghost += "    *******************************\n";
    ghost += "    *******************************\n";
    ghost += "    *****  *******    ******  *****\n";
    ghost += "    ***      *****    ****      ***\n";
    
    // print image
    cout << endl << endl;
    draw.SetColor(7);
    cout << ghost;
    draw.SetColor(7);
    //cout << endl << "      Press any key to start";
    cout << endl << "              PACMAN 2021";

    // play intro
    SFX(game, level, Play::INTRO);

    // any key press starts game
    char input = _getch();
    system("cls");
}
#pragma endregion
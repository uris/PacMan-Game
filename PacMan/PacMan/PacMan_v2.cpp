﻿#include <iostream> // base input / output
#include <string> // manipulate level string
#include <Windows.h> // used to output colors
#include <conio.h> // used to get input without requiring return
#include <thread> // used to get input while running game
#include <chrono> // used for timers
#include <iomanip> // formating output
#include <algorithm> // string transforms

using namespace std;
using namespace std::chrono;

// enum classes -->
enum class Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
    NONE = -1,
};
enum class Mode {
    CHASE = 0,
    ROAM = 1,
    RUN = 2,
    SPAWN = -1,
};

// scaffolding -->
struct Coord {
    int row = 0;
    int col = 0;
};

// globals objects -->
struct Player {
    static constexpr char player = 'C';
    Coord curr_pos = { curr_pos.row = 0, curr_pos.col = 0 };
    Coord spawn_pos = { spawn_pos.row = 0, spawn_pos.col = 0 };
    Coord prev_pos = { prev_pos.row = 0, prev_pos.col = 0 };
    Direction curr_direction = Direction::LEFT;
    Direction prev_direction = Direction::LEFT;
    bool is_super = false;
    bool color_on = false;
    int lives = 3;
    int score = 0;
};
struct Ghost {
    char ghost = 'R';
    char square_content_now = ' ';
    char square_content_prior = square_content_now;
    Coord curr_pos = { curr_pos.row = 0, curr_pos.col = 0 };
    Coord spawn_pos = { spawn_pos.row = 0, spawn_pos.col = 0 };
    Coord prev_pos = { prev_pos.row = 0, prev_pos.col = 0 };
    Coord spawn_target = { spawn_target.row = 10, spawn_target.col = 22 }; // just above spawn area
    Coord roam_target = { roam_target.row = 0, roam_target.col = 0 }; // roams to top right hand side
    Coord chase_mod = { chase_mod.row = 0, chase_mod.col = 0 }; // runs after player position
    Direction curr_direction = Direction::UP;
    Direction prev_direction = Direction::UP;
    Mode mode = Mode::CHASE;
    bool run_first_move = true;
    bool is_edible = false;
    bool color_on = false;
    int color = 455;
    int rand = 5; // percent of time it will make random move choice
    int wait = 0;
    bool skip_turn = false; // use this to slow down moster if edible
    int look_ahead = 5; // how far ahead the IA looks for player
};
struct Level   {
    // Colors
    static constexpr int cINVISIBLE = 0; // black on black
    static constexpr int cWHITE = 7; // white
    static constexpr int cPLAYER = 14; // yellow
    static constexpr int cWALLS = 392; // gray on gray text
    // Ghosts
    static constexpr int gRED = 0;
    static constexpr int gYELLOW = 1;
    static constexpr int gBLUE = 2;
    static constexpr int gPINK = 3;
    // level info and size
    string title = "Scene 1";
    char** p_map = nullptr;
    //char map[23][47]{};
    int rows = 23;
    int cols = 47;
    bool level_paused = true;
    // teleport coords and direction
    Coord tp_1 = { tp_1.row = 0, tp_1.col = 0 };
    Coord tp_2 = { tp_2.row = 0, tp_2.col = 0 };
    Coord ghost_spawn = { ghost_spawn.row = 0, ghost_spawn.col = 0 };
    bool tp_row = true;
    // level chars
    static constexpr char powerup = 254; // powerup ascii
    static constexpr char pellet = 250; // pellet ascii
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
    // status info
    int total_pellets = 0; // totala level pellets
    int eaten_pellets = 0; // pellets consumed
    int eaten_ghots = 0; // pellets consumed
    int edible_ghost_duration = 20; // seconds
    int points_pellet = 10;
    int points_ghost = 250;
    int all_ghost_bonus = 2600;
    bool is_complete = false;
    // Manange modes
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
    Mode level_mode = Mode::CHASE;
};
struct Game {
    bool game_over = false;
    bool gobble_pause = false;
    bool player_beat_pause = false;
    int gobble_delay = 500; // wait in milliseconds
    int player_beat_delay = 1000; // wait in milliseconds
    int refresh_delay = 60; // milliseconds
    int ghost_color_on = 275; // for ghost flash
    int ghost_color_off = 155; // for ghost flash
};

// Functions -->

// level setup, display and management
void SetUp(Game& game, Level& level, Player& player, Ghost ghosts[], const int scene);
char** CreateLevelScene(Level& level, Player& player, Ghost ghosts[], const int scene);
void DrawLevel(Game& game, Level& level, Player& player, Ghost ghosts[]);
void StatusBar(Game& game, Level& level, Player& player, Ghost ghosts[]);
void SpawnMonster(Level& level, Ghost ghosts[]);
void SpawnMonster(Level& level, Ghost ghosts[], Ghost ghost, bool playerDied);
void SpawnPlayer(Player& player);

// Player movement
void GetPlayerDirection(Game& game, Level& level, Player& player);
void MovePlayer(Game& game, Level& level, Player& player, Ghost ghosts[]);
Coord NewPos(const Coord& currentPos, const Direction& direction);

// Ghost movement
int MoveMonster(Game& game, Level& level, Player& player, Ghost& ghost, Ghost ghosts[]);
void DoMonsterMove(Level& level, Player& player, Ghost ghosts[], Ghost& ghost, Direction bestMove);
int GetBestMove(Level& level, Player& player, Ghost& ghost, Coord move, Direction curr_direction, int depth, bool isGhost);
bool NotWall(const Level& level, const Coord& move, const Direction& direction);
bool CanMove(Level& level, Ghost& ghost, Coord move, Direction direction, Direction curr_direction);
Coord NewMove(Level& level, Ghost& ghost, Coord move, Direction direction);
bool IsTeleport(const Level& level, const Coord& move);
Direction RandomGhostMove(Level& level, Ghost& ghost);
char GetSquareContentNow(Level& level, Ghost ghosts[], Coord square);
Coord GhostTeleport(Level& level, Ghost& ghost, Coord move, Direction direction);

// Player/Ghost Events and Status
void PlayerMonsterCollision(Game& game, Level& level, Player& player, Ghost ghosts[]);
bool CheckCollision(Player& player, Ghost& ghost);
void SetPlayerState(Game& game, Level& level, Player& player, Ghost ghosts[]);
void SetGhostMode(Level& level, Player& player, Ghost ghosts[]);

// Game management
void CheckLevelComplete(Level& level);
void RefreshDelay(Game& game, Level& level);
void PlayerMonsterCollisionDelay(Game& game, Level& level);
void ShowConsoleCursor(bool showFlag);
void DeallocateMem(Level& level);

// Helper and scaffolding
string TransformString(string text, int operation);
void SetColor(int color);
void ShowColors(int colors);
void TopLeft(int rows);
string GhostMode(Ghost& ghost);
void Credits();
void ReplaceString(string& text, string from, char to);
Coord MapSize(const string& map);
void ShowConsoleCursor(bool showFlag);

// ************

int main()
{
    //ShowColors(500); // find colors to use
 
    Credits(); // show credits
    int scene = 1; // start at Scene 1

    do
    {
        Game game;
        Level level;
        Player player;
        Ghost redGhost, yellowGhost, orangeGhost, pinkGhost;
        Ghost ghosts[4] = { redGhost, yellowGhost, orangeGhost, pinkGhost };
               
        // Set up variables and data
        SetUp(game, level, player, ghosts, scene);

        // remove cursor from view for less flicker during game
        ShowConsoleCursor(false);

        // draw initial level state
        DrawLevel(game, level, player, ghosts);

        // start a thread to get input while the main program executes
        thread inputThread(GetPlayerDirection, ref(game), ref(level), ref(player)); // new the ref() wrapper to pass by ref into thread

        do
        {
            // move the player
            MovePlayer(game, level, player, ghosts);

            // process any player / ghost collision
            PlayerMonsterCollision(game, level, player, ghosts);

            // set ghost chase modes as approprite
            SetGhostMode(level, player, ghosts);

            //// move the ghost
            MoveMonster(game, level, player, redGhost, ghosts);

            // process any player / ghost collision again
            PlayerMonsterCollision(game, level, player, ghosts);

            // delay render if there's a collision
            PlayerMonsterCollisionDelay(game, level);

            // Draw the level current state
            DrawLevel(game, level, player, ghosts);

            // display stats and number lives
            StatusBar(game, level, player, ghosts);

            // end condition for the gmae once user has eaten all pellets
            CheckLevelComplete(level);

            // introduce a wait for fast PC
            RefreshDelay(game, level);

        } while (!level.is_complete && !game.game_over);

        // when game ends, wait for the input thread rejoin the main thread
        inputThread.join();

        // bring cursor fro the console back into view
        ShowConsoleCursor(false);

        if (game.game_over)
        {
            cout << "       play again? 'y' = yes, 'n' = no         ";
            char input = _getch();
            if (input == 'n')
            {
                DeallocateMem(level);
                break;
            }
            else
            {
                system("cls");
                scene = 1;
            }
        }
        else
        {
            system("cls");
            scene = 2;
        }

    } while (true);

    return 0;
}

// level setup, display and management
void SetUp(Game& game, Level& level, Player& player, Ghost ghosts[], int scene)
{
    // name level
    level.level_paused = true;
    switch (scene)
    {
    case 1:
        level.title = "Scene 1";
        break;
    case 2:
        level.title = "Scene 2";
        break;
    default:
        level.title = "Scene 1";
        scene = 1;
        break;
    }

    // set lives
    player.lives = 3;

    // pause level
    level.level_paused = true;
    
    // initialize ghosts
    SpawnMonster(level, ghosts);
    
    // create level maze, set level params and set up ghosts and players
    level.p_map = CreateLevelScene(level, player, ghosts, scene);

    // start timer beggining with the chase mode
    level.chase_time_start = chrono::high_resolution_clock::now();
}
void SpawnMonster(Level& level, Ghost ghosts[])
{
    // spawn red ghost
    ghosts[0].ghost = Level::red_ghost;
    ghosts[0].square_content_now = ' ';
    ghosts[0].square_content_prior = ghosts[0].square_content_now;
    ghosts[0].curr_pos = { ghosts[0].curr_pos.row = 0, ghosts[0].curr_pos.col = 0 };
    ghosts[0].spawn_pos = { ghosts[0].spawn_pos.row = 0, ghosts[0].spawn_pos.col = 0 };
    ghosts[0].prev_pos = { ghosts[0].prev_pos.row = 0, ghosts[0].prev_pos.col = 0 };
    ghosts[0].spawn_target = { ghosts[0].spawn_target.row = level.ghost_spawn.row, ghosts[0].spawn_target.col = level.ghost_spawn.row }; // just above spawn area
    ghosts[0].roam_target = { ghosts[0].roam_target.row = 24, ghosts[0].roam_target.col = 45 }; // roams to top right hand side
    ghosts[0].chase_mod = { ghosts[0].chase_mod.row = 0, ghosts[0].chase_mod.col = 0 };
    ghosts[0].curr_direction = Direction::UP;
    ghosts[0].prev_direction = Direction::UP;
    ghosts[0].mode = Mode::SPAWN;
    ghosts[0].run_first_move = true;
    ghosts[0].is_edible = false;
    ghosts[0].color_on = false;
    ghosts[0].color = 71;
    ghosts[0].rand = 5; // percent of time it will make random move choice
    ghosts[0].wait = 15;
    ghosts[0].skip_turn = false; // use this to slow down moster if edible
    ghosts[0].look_ahead = 5; // how far ahead the IA looks for player

    // spawn yellow ghost
    ghosts[1].ghost = Level::yellow_ghost;
    ghosts[1].square_content_now = ' ';
    ghosts[1].square_content_prior = ghosts[1].square_content_now;
    ghosts[1].curr_pos = { ghosts[1].curr_pos.row = 0, ghosts[1].curr_pos.col = 0 };
    ghosts[1].spawn_pos = { ghosts[1].spawn_pos.row = 0, ghosts[1].spawn_pos.col = 0 };
    ghosts[1].prev_pos = { ghosts[1].prev_pos.row = 0, ghosts[1].prev_pos.col = 0 };
    ghosts[1].spawn_target = { ghosts[0].spawn_target.row = level.ghost_spawn.row, ghosts[0].spawn_target.col = level.ghost_spawn.row }; // just above spawn area
    ghosts[1].roam_target = { ghosts[1].roam_target.row = 24, ghosts[1].roam_target.col = 2 }; // roams to top right hand side
    ghosts[1].chase_mod = { ghosts[1].chase_mod.row = 0, ghosts[1].chase_mod.col = 3 };
    ghosts[1].curr_direction = Direction::UP;
    ghosts[1].prev_direction = Direction::UP;
    ghosts[1].mode = Mode::SPAWN;
    ghosts[1].run_first_move = true;
    ghosts[1].is_edible = false;
    ghosts[1].color_on = false;
    ghosts[1].color = 367;
    ghosts[1].rand = 5; // percent of time it will make random move choice
    ghosts[1].wait = 30;
    ghosts[1].skip_turn = false; // use this to slow down moster if edible
    ghosts[1].look_ahead = 5; // how far ahead the IA looks for player

    // spawn blue ghost
    ghosts[2].ghost = Level::blue_ghost;
    ghosts[2].square_content_now = ' ';
    ghosts[2].square_content_prior = ghosts[2].square_content_now;
    ghosts[2].curr_pos = { ghosts[2].curr_pos.row = 0, ghosts[2].curr_pos.col = 0 };
    ghosts[2].spawn_pos = { ghosts[2].spawn_pos.row = 0, ghosts[2].spawn_pos.col = 0 };
    ghosts[2].prev_pos = { ghosts[2].prev_pos.row = 0, ghosts[2].prev_pos.col = 0 };
    ghosts[2].spawn_target = { ghosts[0].spawn_target.row = level.ghost_spawn.row, ghosts[0].spawn_target.col = level.ghost_spawn.row }; // just above spawn area
    ghosts[2].roam_target = { ghosts[2].roam_target.row = -3, ghosts[2].roam_target.col = 2 }; // roams to top right hand side
    ghosts[2].chase_mod = { ghosts[2].chase_mod.row = 0, ghosts[2].chase_mod.col = -3 };
    ghosts[2].curr_direction = Direction::UP;
    ghosts[2].prev_direction = Direction::UP;
    ghosts[2].mode = Mode::SPAWN;
    ghosts[2].run_first_move = true;
    ghosts[2].is_edible = false;
    ghosts[2].color_on = false;
    ghosts[2].color = 435;
    ghosts[2].rand = 5; // percent of time it will make random move choice
    ghosts[2].wait = 45;
    ghosts[2].skip_turn = false; // use this to slow down moster if edible
    ghosts[2].look_ahead = 5; // how far ahead the IA looks for player

    // spawn pink ghost
    ghosts[3].ghost = Level::pink_ghost;
    ghosts[3].square_content_now = ' ';
    ghosts[3].square_content_prior = ghosts[3].square_content_now;
    ghosts[3].curr_pos = { ghosts[3].curr_pos.row = 0, ghosts[3].curr_pos.col = 0 };
    ghosts[3].spawn_pos = { ghosts[3].spawn_pos.row = 0, ghosts[3].spawn_pos.col = 0 };
    ghosts[3].prev_pos = { ghosts[3].prev_pos.row = 0, ghosts[3].prev_pos.col = 0 };
    ghosts[3].spawn_target = { ghosts[0].spawn_target.row = level.ghost_spawn.row, ghosts[0].spawn_target.col = level.ghost_spawn.row }; // just above spawn area
    ghosts[3].roam_target = { ghosts[3].roam_target.row = -3, ghosts[3].roam_target.col = 45 }; // roams to top right hand side
    ghosts[3].chase_mod = { ghosts[3].chase_mod.row = -3, ghosts[3].chase_mod.col = 0 };
    ghosts[3].curr_direction = Direction::UP;
    ghosts[3].prev_direction = Direction::UP;
    ghosts[3].mode = Mode::SPAWN;
    ghosts[3].run_first_move = true;
    ghosts[3].is_edible = false;
    ghosts[3].color_on = false;
    ghosts[3].color = 479;
    ghosts[3].rand = 5; // percent of time it will make random move choice
    ghosts[3].wait = 60;
    ghosts[3].skip_turn = false; // use this to slow down moster if edible
    ghosts[3].look_ahead = 5; // how far ahead the IA looks for player
}
void SpawnMonster(Level& level, Ghost ghosts[], Ghost ghost, bool playerDied)
{
    switch (ghost.ghost)
    {
    case Level::red_ghost:
        // spawn red ghost
        ghosts[0].square_content_now = ' ';
        ghosts[0].square_content_prior = ghosts[0].square_content_now;
        ghosts[0].curr_pos = { ghosts[0].curr_pos.row = ghosts[0].spawn_pos.row,  ghosts[0].curr_pos.col = ghosts[0].spawn_pos.col };
        ghosts[0].prev_pos = { ghosts[0].prev_pos.row = ghosts[0].curr_pos.row, ghosts[0].prev_pos.col = ghosts[0].curr_pos.col };
        ghosts[0].curr_direction = Direction::UP;
        ghosts[0].prev_direction = Direction::UP;
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
        ghosts[1].curr_pos = { ghosts[1].curr_pos.row = ghosts[1].spawn_pos.row, ghosts[1].curr_pos.col = ghosts[1].spawn_pos.col };
        ghosts[1].prev_pos = { ghosts[1].prev_pos.row = ghosts[1].curr_pos.row, ghosts[1].prev_pos.col = ghosts[1].curr_pos.col };
        ghosts[1].curr_direction = Direction::UP;
        ghosts[1].prev_direction = Direction::UP;
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
        ghosts[2].curr_pos = { ghosts[2].curr_pos.row = ghosts[2].spawn_pos.row, ghosts[2].curr_pos.col = ghosts[2].spawn_pos.col };
        ghosts[2].prev_pos = { ghosts[2].prev_pos.row = ghosts[2].curr_pos.row, ghosts[2].prev_pos.col = ghosts[2].curr_pos.col };
        ghosts[2].curr_direction = Direction::UP;
        ghosts[2].prev_direction = Direction::UP;
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
        ghosts[3].curr_pos = { ghosts[3].curr_pos.row = ghosts[3].spawn_pos.row, ghosts[3].curr_pos.col = ghosts[3].spawn_pos.col };
        ghosts[3].prev_pos = { ghosts[3].prev_pos.row = ghosts[3].curr_pos.row, ghosts[3].prev_pos.col = ghosts[3].curr_pos.col };
        ghosts[3].curr_direction = Direction::UP;
        ghosts[3].prev_direction = Direction::UP;
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
char** CreateLevelScene(Level& level, Player& player, Ghost ghosts[], int scene)
{
    // create string with the level - this makes it easier to convert a string diagram to a level array
    // must be of the level dimensions 17 rows by 23 cols
    string map, level1, level2;

    level1 =  "+---------------------------------------------+";
    level1 += "|%o...................%|%...................o%|";
    level1 += "|%.%-------%.%------%.%|%.%------%.%-------%.%|";
    level1 += "|%.%#######%.%######%.%|%.%######%.%#######%.%|";
    level1 += "|%.%-------%.%------%.%|%.%------%.%-------%.%|";
    level1 += "|%...........................................%|";
    level1 += "|%.%-------%.%|%.%-----------%.%|%.%-------%.%|";
    level1 += "|%.%-------%.%|%.%-----------%.%|%.%-------%.%|";
    level1 += "|%...........%|%......%|%......%|%...........%|";
    level1 += "|%.%-------%.%------%.%-%.%------%.%-------%.%|";
    level1 += "|%.%#######%.%|%.......^.......%|%.%#######%.%|";
    level1 += "|%.%-------%.%|%.%----$$$----%.%|%.%-------%.%|";
    level1 += "T ...........%|%.%| B R Y P |%.%|%........... T";
    level1 += "|%.%-------%.%|%.%-----------%.%|%.%-------%.%|";
    level1 += "|%.%#######%.%|%...............%|%.%#######%.%|";
    level1 += "|%.%#######%.%|%.%-----------%.%|%.%#######%.%|";
    level1 += "|%.%-------%.%|%.%-----------%.%|%.%-------%.%|";
    level1 += "|%.....................S.....................%|";
    level1 += "|%.%-------%.%------%.%|%.%------%.%-------%.%|";
    level1 += "|%.%#######%.%######%.%|%.%######%.%#######%.%|";
    level1 += "|%.%-------%.%------%.%|%.%------%.%-------%.%|";
    level1 += "|%o...................%|%...................o%|";
    level1 += "+---------------------------------------------+";

    level2 =  "+---------------------------------------------+";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "|...o.....................................o...|";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "|.....................^.......................|";
    level2 += "|................----$$$----..................|";
    level2 += "T................| B R Y P |..................T";
    level2 += "|................-----------..................|";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "|......................S......................|";
    level2 += "|...o.....................................o...|";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "|.............................................|";
    level2 += "+---------------------------------------------+";

    switch (scene)
    {
    case 1:
        map = level1;
        break;
    case 2:
        map = level2;
        break;
    default:
        map = level1;
        break;
    }

    // parse through string to replace pellt and powerup markers to their ascii code
    ReplaceString(map, ".", Level::pellet);
    ReplaceString(map, "o", Level::powerup);

    // create dynamic two dimension pointer array to hold map
    Coord size = MapSize(map); // get width and height of the map
    level.cols = size.col; // set lsizes in level
    level.rows = size.row; // set sizes in level
    char** p_mapArray = new char*[size.row];
    for (int i = 0; i < size.row; i++)
    {
        p_mapArray[i] = new char[size.col];
    }
    
    // parse through map string and convert into the level map array
    // '|', '+' = wall; '%' = blank spot with collision; 'T' = portal; 'S' = player starting pos; 'M' = monsters; $ = one way exit only
    int t_row = 0, t_col = 0;

    // iterate through the characters of the map string
    for (string::size_type i = 0; i < map.size(); i++)
    {
        // add to dynamic map array
        p_mapArray[t_row][t_col] = map[i];

        // count pellets that determine end game condition
        if (map[i] == Level::pellet || map[i] == Level::powerup)
            level.total_pellets++;

        // Set player start to position 'S'
        if (map[i] == Level::player_start)
        {
            player.spawn_pos.row = player.curr_pos.row = player.prev_pos.row = t_row;
            player.spawn_pos.col = player.curr_pos.col = player.prev_pos.col = t_col;
        }

        // Set ghost spawn target location
        if (map[i] == Level::ghost_spawn_target)
        {
            level.ghost_spawn.row = t_row;
            level.ghost_spawn.col = t_col;
            p_mapArray[t_row][t_col] = Level::pellet;
            level.total_pellets++;
            
            // set ghosts spawn target
            for (int g = 0; g < 4; g++) // loop through ghosts
            {
                ghosts[g].spawn_target.row = t_row;
                ghosts[g].spawn_target.col = t_col;
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
            ghosts[ghostIndex].spawn_pos.row = ghosts[ghostIndex].curr_pos.row = ghosts[ghostIndex].prev_pos.row = t_row;
            ghosts[ghostIndex].spawn_pos.col = ghosts[ghostIndex].curr_pos.col = ghosts[ghostIndex].prev_pos.col = t_col;
        }
        

        // Set teleport coords for T1 and T2
        if (map[i] == 'T') {
            if (t_row == 0 || t_col == 0) {
                level.tp_1.col = t_col;
                level.tp_1.row = t_row;
            }
            else {
                level.tp_2.col = t_col;
                level.tp_2.row = t_row;
            }
            // set teleport direction
            if (t_col == 0) {
                level.tp_row = true; // teleport across
            }
            else if (t_row == 0) {
                level.tp_row = false; // teleport certically
            }
        }

        // if reached end of map columns process next row
        if (t_col == level.cols - 1) {
            t_col = 0;
            t_row++;
        }
        else
        {
            t_col++; // increment columns count
        }
    }

    // return map array pointer
    return p_mapArray;
}
void SpawnPlayer(Player& player)
{
    player.curr_pos.row = player.spawn_pos.row;
    player.curr_pos.col = player.spawn_pos.col;
    player.curr_direction = Direction::LEFT;
    player.prev_direction = Direction::LEFT;
    player.is_super = false;
    player.color_on = false;
}
void DrawLevel(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    
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
    TopLeft(level.rows + 5); // + 5 for title and status

    // remove player from map at last position if they are different
    if (player.prev_pos.row != player.curr_pos.row || player.prev_pos.col != player.curr_pos.col)
        level.p_map[player.prev_pos.row][player.prev_pos.col] = Level::space;

    // player in tunnel
    if (player.curr_pos.row == level.tp_1.row && player.curr_pos.col == level.tp_1.col) {
        level.p_map[player.curr_pos.row][player.curr_pos.col] = Level::teleport;
        player.curr_pos.col = level.tp_2.col;
    }
    else if (player.curr_pos.row == level.tp_2.row && player.curr_pos.col == level.tp_2.col) {
        level.p_map[player.curr_pos.row][player.curr_pos.col] = Level::teleport;
        player.curr_pos.col = level.tp_1.col;
    }
    
    for (int g = 0; g < 4; g++) // loop through ghots
    {
        // remove ghosts from map at last position if they are different
        if (ghosts[g].prev_pos.row != ghosts[g].curr_pos.row || ghosts[g].prev_pos.col != ghosts[g].curr_pos.col)
            level.p_map[ghosts[g].prev_pos.row][ghosts[g].prev_pos.col] = Level::space;
        
        // ghost in tunnel
        if (!ghosts[g].skip_turn)
        {
            if (ghosts[g].curr_pos.row == level.tp_1.row && ghosts[g].curr_pos.col == level.tp_1.col) {
                level.p_map[ghosts[g].curr_pos.row][ghosts[g].curr_pos.col] = Level::teleport;
                ghosts[g].curr_pos.col = level.tp_2.col;
            }
            else if (ghosts[g].curr_pos.row == level.tp_2.row && ghosts[g].curr_pos.col == level.tp_2.col) {
                level.p_map[ghosts[g].curr_pos.row][ghosts[g].curr_pos.col] = Level::teleport;
                ghosts[g].curr_pos.col = level.tp_1.col;
            }
        }
        
    }
   
    // Level Title
    SetColor(Level::cWHITE);
    cout << endl << "                 PACMAN: " << TransformString(level.title, 0) << endl << endl;

    // draw current level map
    for (int r = 0; r < level.rows; r++)
    {
        for (int c = 0; c < level.cols; c++)
        {
            // position player
            if (r == player.curr_pos.row && c == player.curr_pos.col)
                level.p_map[r][c] = player.player;

            for (int g = 0; g < 4; g++) // loop through ghots
            {
                // position ghost
                if (r == ghosts[g].curr_pos.row && c == ghosts[g].curr_pos.col)
                    level.p_map[r][c] = ghosts[g].ghost;

                // if the ghost moved from the last position
                if (ghosts[g].prev_pos.row != ghosts[g].curr_pos.row || ghosts[g].prev_pos.col != ghosts[g].curr_pos.col)
                {
                    // put back the content of the square the ghost was last at 
                    if (r == ghosts[g].prev_pos.row && c == ghosts[g].prev_pos.col)
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
                SetColor(Level::cINVISIBLE); // black on black = not visible
                break;
            case Level::pellet: // . = pellet
                current_pellets++;
                SetColor(Level::cWHITE); // gray for bullets
                break;
            case Level::powerup: // o = power up
                current_pellets++;
                SetColor(Level::cWHITE); // white for power ups
                break;
            case Level::pink_ghost: // blue ghost
                if (ghosts[Level::gPINK].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[Level::gPINK].color_on ? SetColor(game.ghost_color_on) : SetColor(game.ghost_color_off);
                    ghosts[Level::gPINK].color_on = !ghosts[Level::gPINK].color_on;
                }
                else { // solid color
                    SetColor(ghosts[Level::gPINK].color);
                }
                break;
            case Level::yellow_ghost: // yellow ghost
                if (ghosts[Level::gYELLOW].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[Level::gYELLOW].color_on ? SetColor(game.ghost_color_on) : SetColor(game.ghost_color_off);
                    ghosts[Level::gYELLOW].color_on = !ghosts[Level::gYELLOW].color_on;
                } else { // solid color
                    SetColor(ghosts[Level::gYELLOW].color);
                }
                break;
            case Level::blue_ghost: // green ghost
                if (ghosts[Level::gBLUE].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[Level::gBLUE].color_on ? SetColor(game.ghost_color_on) : SetColor(game.ghost_color_off);
                    ghosts[Level::gBLUE].color_on = !ghosts[Level::gBLUE].color_on;
                }
                else { // solid color
                    SetColor(ghosts[Level::gBLUE].color);
                }
                break;
            case Level::red_ghost: // red ghost
                if (ghosts[Level::gRED].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[Level::gRED].color_on ? SetColor(game.ghost_color_on) : SetColor(game.ghost_color_off);
                    ghosts[Level::gRED].color_on = !ghosts[Level::gRED].color_on;
                } else { // solid color
                    SetColor(ghosts[Level::gRED].color);
                }
                break;
            case Player::player: // player
                SetColor(Level::cPLAYER);
                break;
            default:
                SetColor(Level::cWALLS); // gray bg on gray text for all other chars making them walls essentially
                break;
            }

            // print char
            cout << level.p_map[r][c];

            // set color back to default
            SetColor(Level::cWHITE);
        }
        // end of row ad line feed
        cout << endl;
    }

    // before start of level get any key to start
    if (level.level_paused) {
        cout << endl;
        cout << "           Press any key to start.";
        char input = _getch();
        level.level_paused = false;
    }

    // ********
    // BUG FIX - brute force - need to look at why we are dropping a few pellets
    level.eaten_pellets = level.eaten_pellets + (level.total_pellets - level.eaten_pellets - current_pellets);

}
void StatusBar(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    // get number of lives
    string lives;
    for (int i = 0; i < player.lives; i++)
    {
        lives = lives + player.player;
    }
    player.score = ((level.eaten_pellets + 1) * level.points_pellet) + (level.eaten_ghots * level.points_ghost) + (level.eaten_ghots >= 4 ? level.all_ghost_bonus : 0);
    cout << endl;
    SetColor(7);
    cout << "    LIVES:";
    SetColor(14);
    cout << (player.lives > 0 ? lives : "-");
    SetColor(7);
    cout << "   SCORE:";
    SetColor(14);
    cout << setfill('0') << setw(8) << player.score;
    SetColor(7);
    cout << "   GHOSTS:";
    SetColor(14);
    cout << (level.eaten_ghots >= 4 ? "*" : "") << "x" << level.eaten_ghots << (level.eaten_ghots >= 4 ? "*" : "");
    SetColor(7);
    cout << "       ";
    cout << "\r";
}

// Player movement
void GetPlayerDirection(Game& game, Level& level, Player& player)
{
    do
    {
        char input = _getch();
        switch (input)
        {
        case 'a':
            player.curr_direction = Direction::LEFT;
            break;
        case 'd':
            player.curr_direction = Direction::RIGHT;
            break;
        case 'w':
            player.curr_direction = Direction::UP;
            break;
        case 's':
            player.curr_direction = Direction::DOWN;
            break;
        default:
            break;
        }
    } while (!level.is_complete && !game.game_over);
}
void MovePlayer(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    player.prev_pos.row = player.curr_pos.row;
    player.prev_pos.col = player.curr_pos.col;

    Coord currentPos, nextPos;
    currentPos = { player.curr_pos.row, player.curr_pos.col };
    nextPos = NewPos(currentPos, player.prev_direction);

    switch (player.curr_direction)
    {
    case Direction::UP: //up
        if (NotWall(level, NewPos(currentPos, Direction::UP), Direction::UP)) {
            nextPos = NewPos(currentPos, Direction::UP);
            player.prev_direction = Direction::UP;
        }
        break;
    case Direction::RIGHT: // right
        if (NotWall(level, NewPos(currentPos, Direction::RIGHT), Direction::RIGHT)) {
            nextPos = NewPos(currentPos, Direction::RIGHT);
            player.prev_direction = Direction::RIGHT;
        }
        break;
    case Direction::DOWN: // down
        if (NotWall(level, NewPos(currentPos, Direction::DOWN), Direction::DOWN)) {
            nextPos = NewPos(currentPos, Direction::DOWN);
            player.prev_direction = Direction::DOWN;
        }
        break;
    case Direction::LEFT: // left
        if (NotWall(level, NewPos(currentPos, Direction::LEFT), Direction::LEFT)) {
            nextPos = NewPos(currentPos, Direction::LEFT);
            player.prev_direction = Direction::LEFT;
        }
        break;
    }

    if (!NotWall(level, nextPos, player.prev_direction)) {
            nextPos = currentPos;
    }

    // move the player
    player.curr_pos.row = nextPos.row;
    player.curr_pos.col = nextPos.col;

    // set any statuses that need setting
    SetPlayerState(game, level, player, ghosts);

    // check collision with a ghost and set the ghosts previous row to the new player position
    for (int g = 0; g < 4; g++)
    {
        if (CheckCollision(player, ghosts[g])) {
            ghosts[g].prev_pos.row = player.curr_pos.row;
            ghosts[g].prev_pos.col = player.curr_pos.col;
        }
    }
    
}

// Ghost movement
int GetBestMove(Level& level, Player& player, Ghost& ghost, Coord move, Direction curr_direction, int depth, bool isGhost)
{
    // and on the target the ghost chases: red chases player pos, yellow player pos + 2 cols (to the right of player)
    switch (ghost.mode)
    {
    case Mode::CHASE: // redude distance to player
        if ((abs(ghost.curr_pos.col - (player.curr_pos.col + ghost.chase_mod.col)) + abs(ghost.curr_pos.row - (player.curr_pos.row + ghost.chase_mod.row))) == 0) { // if player and ghost collide return 0 + depth as score
            return 0 + depth; // add depth to get fastest path
        }
        if (depth == ghost.look_ahead) { // if depth X return the distance score, increase this to make the ghost look forward more
            return (abs(ghost.curr_pos.col - (player.curr_pos.col + ghost.chase_mod.col)) + abs(ghost.curr_pos.row - (player.curr_pos.row + ghost.chase_mod.row)) + depth); // add depth to get fastest path
        }
        break;
    case Mode::ROAM: // reduce distance to the ghost's roam target
        if (depth == ghost.look_ahead) {
            return abs(ghost.curr_pos.col - ghost.roam_target.col) + abs(ghost.curr_pos.row - ghost.roam_target.row);
        }
        break;
    case Mode::SPAWN: // target is above the exit area
        int temp = abs(ghost.curr_pos.col - ghost.spawn_target.col) + abs(ghost.curr_pos.row - ghost.spawn_target.row);
        if (temp == 0)
            ghost.mode = (level.level_mode == Mode::RUN ? Mode::CHASE : level.level_mode);
        return temp;
        break;
    }

    if (isGhost) {
        // if its the ghost move we want the lowest possible distance to player
        // right now we won't play the player move unless this doesn't work well.
        int score = 1000, bestScore = ghost.mode == Mode::RUN ? -1000 : 1000;
        Coord nextMove;
        Direction new_direction = Direction::NONE;

        for (int i = 0; i <= 3; i++) // cycle through each next possible move up, down, left, right
        {
            new_direction = static_cast<Direction>(i); // cast index to direction up, down, left, right
            if (CanMove(level, ghost, move, new_direction, curr_direction)) // check if the direction is valid i.e not wall or reverse
            {
                // set new coords of ghost
                nextMove = NewMove(level, ghost, move, new_direction);
                ghost.curr_pos.row = nextMove.row; ghost.curr_pos.col = nextMove.col;

                // calculate distance score based on new coords
                int score = GetBestMove(level, player, ghost, nextMove, new_direction, depth + 1, true); // get the minimax score for the move (recurssive)

                // revert ghost coords back
                ghost.curr_pos.row = move.row; ghost.curr_pos.col = move.col;

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
Coord NewMove(Level& level, Ghost& ghost, Coord move, Direction direction)
{
    // based on direction, calc new coord of the move and return it
    // if moving from a teleport to a teleport make the jump based on whether the teleport is horizontal or vertical
    Coord newCoord;
    switch (direction)
    {
    case Direction::UP:
        (level.tp_row && IsTeleport(level, move)) ? newCoord.row = move.row - 1 : newCoord.row = move.row - 1;
        newCoord.col = move.col;
        break;
    case  Direction::DOWN:
        (level.tp_row && IsTeleport(level, move)) ? newCoord.row = move.row + 1 : newCoord.row = move.row + 1;
        newCoord.col = move.col;
        break;
    case Direction::LEFT:
        (level.tp_row && IsTeleport(level, move)) ? newCoord.col = GhostTeleport(level, ghost, move, direction).col : newCoord.col = move.col - 1;
        newCoord.row = move.row;
        break;
    case  Direction::RIGHT:
        (level.tp_row && IsTeleport(level, move)) ? newCoord.col = GhostTeleport(level, ghost, move, direction).col : newCoord.col = move.col + 1;
        newCoord.row = move.row;
        break;
    }
    return newCoord;
}
bool CanMove(Level& level, Ghost& ghost, Coord move, Direction direction, Direction curr_direction)
{
    // can only do one reverse move if on the run otherwsie no reverse moves
    switch (direction)
    {
    case  Direction::UP:
        if (curr_direction == Direction::DOWN)
        {
            if (ghost.mode == Mode::RUN && ghost.run_first_move)
                ghost.run_first_move = false;
            else
                return false;
        }
        break;
    case  Direction::DOWN:
        if (curr_direction == Direction::UP)
        {
            if (ghost.mode == Mode::RUN && ghost.run_first_move)
                ghost.run_first_move = false;
            else
                return false;
        }
        break;
    case  Direction::LEFT:
        if (curr_direction == Direction::RIGHT)
        {
            if (ghost.mode == Mode::RUN && ghost.run_first_move)
                ghost.run_first_move = false;
            else
                return false;
        }
        break;
    case  Direction::RIGHT:
        if (curr_direction == Direction::LEFT)
        {
            if (ghost.mode == Mode::RUN && ghost.run_first_move)
                ghost.run_first_move = false;
            else
                return false;
        }
        break;
    }

    // see if the next move is viable
    switch (direction)
    {
    case  Direction::UP:
        return (NotWall(level, NewMove(level, ghost, move, Direction::UP), Direction::UP) ? true : false);
        break;
    case  Direction::DOWN:
        return (NotWall(level, NewMove(level, ghost, move, Direction::DOWN), Direction::DOWN) ? true : false);
        break;
    case  Direction::LEFT:
        return (NotWall(level, NewMove(level, ghost, move, Direction::LEFT), Direction::LEFT) ? true : false);
        break;
    case  Direction::RIGHT:
        return (NotWall(level, NewMove(level, ghost, move, Direction::RIGHT), Direction::RIGHT) ? true : false);
        break;
    }

    return 0; // should never really hit this
}

Coord GhostTeleport(Level& level, Ghost& ghost, Coord move, Direction direction)
{
    if (level.tp_1.row == move.row && level.tp_1.col == move.col && ghost.prev_pos.col != level.tp_1.col)
        if(direction == Direction::LEFT)
            return { level.tp_2.row, level.tp_2.col };
        else
            return { move.row, move.col + 1 };

    if (level.tp_2.row == move.row && level.tp_2.col == move.col && ghost.prev_pos.col != level.tp_2.col)
        if (direction == Direction::RIGHT)
            return { level.tp_1.row, level.tp_1.col };
        else
            return { move.row, move.col - 1 };
    return move;
}
Direction RandomGhostMove(Level& level, Ghost& ghost)
{
    
    Direction newDirection = Direction::NONE;
    Coord move = { move.row = ghost.curr_pos.row, move.col = ghost.curr_pos.col };
    int unsigned seed = (int)(std::chrono::system_clock::now().time_since_epoch().count());
    srand(seed);

    if (ghost.run_first_move) {
        // first move when on the run is to reverse direction
        switch (ghost.curr_direction)
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
    }

    do
    {
        if (!ghost.run_first_move)
        {
            if (IsTeleport(level, move))
            {
                if (ghost.curr_direction == Direction::LEFT)
                {
                    newDirection = Direction::LEFT;
                }
                else
                {
                    newDirection = Direction::RIGHT;
                }
            }
            else
            {
                int randomNumber = rand() % 4; //generate random number to select from the 4 possible options
                newDirection = static_cast<Direction>(randomNumber);
            }
        }
        if (CanMove(level, ghost, move, newDirection, ghost.curr_direction))
            return newDirection;
    } while (true);
}
int MoveMonster(Game& game, Level& level, Player& player, Ghost& ghost, Ghost ghosts[])
{
    Direction bestMove = Direction::NONE; // will be the next move

    for (int g = 0; g < 4; g++)
    {
        if (ghosts[g].skip_turn || game.game_over || CheckCollision(player, ghosts[g]))
        {
            continue; // next ghost (no move)
        }

        if (ghosts[g].wait > 0)
        {
            ghosts[g].wait--; // ghost has respawned and we are counting dowm the tics to let it move
            continue; // next ghost (no move)
        }

        if (ghosts[g].mode == Mode::RUN) // make random moves truning around on first move
        {
            bestMove = RandomGhostMove(level, ghosts[g]);
            DoMonsterMove(level, player, ghosts, ghosts[g], bestMove);
            continue; // next ghost
        }

        if (ghosts[g].mode != Mode::RUN)  // run recursive AI for chase, roam and spawn modes 
        {
            int score = 0, bestScore = 1000;
            Coord nextMove;
            Coord move = { move.row = ghosts[g].curr_pos.row, move.col = ghosts[g].curr_pos.col };
            Direction new_direction = Direction::NONE;

            for (int i = 0; i <= 3; i++) // cycle through up,down,left,right to find the valid best next move
            {
                new_direction = static_cast<Direction>(i);
                if (CanMove(level, ghosts[g], move, new_direction, ghosts[g].curr_direction)) // check for next available square
                {
                    // set new coords of ghost
                    nextMove = NewMove(level, ghosts[g], move, new_direction);
                    ghosts[g].curr_pos.row = nextMove.row; ghosts[g].curr_pos.col = nextMove.col;

                    // calculate distance score based on new coords
                    int score = GetBestMove(level, player, ghosts[g], nextMove, new_direction, 1, true); // get the minimax score for the move (recurssive)

                    // revert ghost coords back
                    ghosts[g].curr_pos.row = move.row; ghosts[g].curr_pos.col = move.col;

                    // if the new move score gets the ghost closer to the target coord
                    if (score < bestScore)
                    {
                        bestScore = score; // set new best score
                        bestMove = new_direction; // set new best move direction
                    }

                }
            }
            DoMonsterMove(level, player, ghosts, ghosts[g], bestMove);
            continue; // next ghost
        }
    }

    return 0; // clean exit
}
void DoMonsterMove(Level& level, Player& player, Ghost ghosts[], Ghost& ghost, Direction bestMove)
{
    ghost.prev_pos.row = ghost.curr_pos.row;
    ghost.prev_pos.col = ghost.curr_pos.col;
    ghost.square_content_prior = ghost.square_content_now;

    switch (bestMove)
    {
    case Direction::UP: //up
        ghost.square_content_now = GetSquareContentNow(level, ghosts, { ghost.curr_pos.row -1, ghost.curr_pos.col});
        ghost.curr_pos.row--;
        ghost.curr_direction = Direction::UP;
        break;
    case Direction::RIGHT: // right
        ghost.square_content_now = GetSquareContentNow(level, ghosts, { ghost.curr_pos.row, ghost.curr_pos.col + 1 });
        ghost.curr_pos.col++;
        ghost.curr_direction = Direction::RIGHT;
        break;
    case Direction::DOWN: // down
        ghost.square_content_now = GetSquareContentNow(level, ghosts, { ghost.curr_pos.row + 1, ghost.curr_pos.col});
        ghost.curr_pos.row++;
        ghost.curr_direction = Direction::DOWN;
        break;
    case Direction::LEFT: // left
        ghost.square_content_now = GetSquareContentNow(level, ghosts, { ghost.curr_pos.row, ghost.curr_pos.col - 1 });
        ghost.curr_pos.col--;
        ghost.curr_direction = Direction::LEFT;
        break;
    default:
        break;
    }
    // if the sqaure the ghost moved into is the player
    ghost.square_content_now == Player::player ? ghost.square_content_now = Level::space : ghost.square_content_now;

    // if the mosnter is over the player save a blank space to buffer
    CheckCollision(player, ghost) ? ghost.square_content_now = Level::space : ghost.square_content_now;

}
char GetSquareContentNow(Level& level, Ghost ghosts[], Coord square)
{
    switch (level.p_map[square.row][square.col])
    {
        case Level::pellet:
        case Level::powerup:
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

// Player & Ghost Common Movement functions
bool NotWall(const Level& level, const Coord& move, const Direction& direction)
{
    switch (level.p_map[move.row][move.col])
    {
    case Level::space:
    case Level::teleport:
    case Level::pellet:
    case Level::powerup:
    case Level::red_ghost:
    case Level::yellow_ghost:
    case Level::blue_ghost:
    case Level::pink_ghost:
    case Player::player:
        return true;
        break;
    case Level::one_way: // one way move to exit ghost spawn area
        return (direction != Direction::DOWN ? true : false);
        break;
    }
    return false;
}
Coord NewPos(const Coord& currentPos, const Direction& direction)
{
    Coord newPos;
    newPos.row = currentPos.row;
    newPos.col = currentPos.col;
    switch (direction)
    {
    case Direction::UP: //up
        newPos.row--;
        break;
    case Direction::RIGHT: // right
        newPos.col++;
        break;
    case Direction::DOWN: // down
        newPos.row++;
        break;
    case Direction::LEFT: // left
        newPos.col--;
        break;
    }
    return newPos;
}
bool IsTeleport(const Level& level, const Coord& move)
{
    if (level.tp_1.row == move.row && level.tp_1.col == move.col)
        return true;
    if (level.tp_2.row == move.row && level.tp_2.col == move.col)
        return true;
    return false;
}

// Player/Ghost Events and Status
void SetPlayerState(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    char levelObj = level.p_map[player.curr_pos.row][player.curr_pos.col];
    switch (levelObj)
    {
    case Level::pellet: // eat pellet
        level.eaten_pellets++;
        break;
    case Level::powerup: // eat power up
        for (int g = 0; g < 4; g++) // loop ghosts
        {
            ghosts[g].mode != Mode::SPAWN ? ghosts[g].is_edible = true : ghosts[g].is_edible = false; // ghost only edible on power up if out of spwan area
            ghosts[g].is_edible ? ghosts[g].mode = Mode::RUN : ghosts[g].mode = ghosts[g].mode;
            ghosts[g].mode == Mode::RUN ? ghosts[g].run_first_move = true : ghosts[g].run_first_move = false;
        }
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
        if (ghosts[g].curr_pos.row == level.ghost_spawn.row && ghosts[g].curr_pos.col == level.ghost_spawn.col)
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
                if (ghosts[g].square_content_now == Level::pellet || ghosts[g].square_content_now == Level::powerup)
                    level.eaten_pellets++;
                level.eaten_ghots++; // increment ghosts eaten
                game.gobble_pause = true; // set small pause after eating ghost
                SpawnMonster(level, ghosts, ghosts[g], false); // reset ghost to spawn area whihc resets mode to spawn
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
        player.lives--;
        if (player.lives > 0)
        {
            game.player_beat_pause = true; // give player time to get ready
            SpawnPlayer(player);
            for (int g = 0; g < 4; g++) {
                // replace ghost with empty sqaure since the player was there an any pellet got eaten
                level.p_map[ghosts[g].curr_pos.row][ghosts[g].curr_pos.col] = ' ';
                // respawn ghost
                SpawnMonster(level, ghosts, ghosts[g], true);
            }
        }
        else
        {
            game.game_over = true;
        }
    }

}
bool CheckCollision(Player& player, Ghost& ghost)
{
    return(player.curr_pos.col == ghost.curr_pos.col && player.curr_pos.row == ghost.curr_pos.row ? true : false);
}

// Game management
void RefreshDelay(Game& game, Level& level)
{
    if (!level.is_complete && !game.game_over)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(game.refresh_delay)); // pause to slow game
    }
}
void PlayerMonsterCollisionDelay(Game& game, Level& level)
{
    if (game.gobble_pause)
    {
        game.gobble_pause = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(game.gobble_delay)); // pause for ghost gobble
    }
    else if (game.player_beat_pause)
    {
        game.player_beat_pause = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(game.player_beat_delay)); // pause for player gobble
    }
}
void CheckLevelComplete(Level& level)
{
    level.eaten_pellets == level.total_pellets ? level.is_complete = true : level.is_complete = false; // if all pellets are eaten game ends - 339 in this map
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

// Helper and scaffolding
void SetColor(int color)
{
    // windows only - sets text color for command line
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void ShowColors(int colors)
{
    for (int i = 0; i < colors; i++)
    {
        SetColor(i);
        cout << "This is color " << i << endl;
    }
}
void TopLeft(int rows)
{
    for (int i = 0; i < rows; i++) // up one line
    {
        cout << "\x1b[A";
    }
    cout << "\r";
}
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
void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}
void ReplaceString(string& text, const string from, const char to)
{
    int start_pos = 0;
    string to_string = { to };
    while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
    text.replace(start_pos, from.length(), to_string);
    start_pos += to_string.length(); // ...
    }
}
Coord MapSize(const string& map)
{
    int rows = 0, cols = 0;
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
    return{ rows, cols };
}


// credits
void Credits()
{
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
    
    cout << endl << endl;
    SetColor(7);
    cout << ghost;
    SetColor(7);
    //cout << endl << "      Press any key to start";
    cout << endl << "              PACMAN 2021";
    char input = _getch();
    system("cls");
    
}
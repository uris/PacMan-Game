#include <iostream> // base input / output
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
struct Level {
    // level info and size
    string title = "Scene 1";
    string author = "Uris";
    char map[23][47]{};
    int rows = 23;
    int cols = 47;
    // teleport coords and direction
    Coord tp_1 = { tp_1.row = 0, tp_1.col = 0 };
    Coord tp_2 = { tp_2.row = 0, tp_2.col = 0 };
    bool tp_row = true;
    // level status and chars
    static constexpr char chr_powerup = 'o';
    static constexpr char chr_pellet = '.';
    int total_pellets = 0; // totala level pellets
    int eaten_pellets = 0; // pellets consumed
    int eaten_ghots = 0; // pellets consumed
    int edible_ghost_duration = 20; // seconds
    int points_pellet = 10;
    int points_ghost = 250;
    int all_ghost_bonus = 2600;
    bool is_complete = false;
    // Manange modes
    int roam_count = 6;
    int roam_for = 15; // seconds
    int chase_for = 15; // seconds
    int run_for = 20; // seconds
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
void SetUp(Game& game, Level& level, Player& player, Ghost ghosts[]);
void CreateLevelScene(Level& level, Player& player, Ghost ghosts[]);
void DrawLevel(Game& game, Level& level, Player& player, Ghost ghosts[]);
void StatusBar(Game& game, Level& level, Player& player, Ghost ghosts[]);
void SpawnMonster(Ghost ghosts[]);
void SpawnMonster(Ghost ghosts[], Ghost ghost, bool playerDied);
void SpawnPlayer(Player& player);

// Player movement
void GetPlayerDirection(Game& game, Level& level, Player& player);
void MovePlayer(Game& game, Level& level, Player& player, Ghost ghosts[]);
void KeepMovePlayer(Game& game, Level& level, Player& player, Ghost ghosts[]);
bool PlayerCanMove(Level& level, int newPRow, int newPCol);

// Ghost movement
int MoveMonster(Game& game, Level& level, Player& player, Ghost& ghost, Ghost ghosts[]);
void DoMonsterMove(Level& level, Player& player, Ghost ghosts[], Ghost& ghost, Direction bestMove);
int GetBestMove(Level& level, Player& player, Ghost& ghost, Coord move, Direction curr_direction, int depth, bool isGhost);
bool NotWall(Level& level, Coord move, Direction direction);
bool CanMove(Level& level, Ghost& ghost, Coord move, Direction direction, Direction curr_direction);
Coord NewMove(Level& level, Ghost& ghost, Coord move, Direction direction);
bool IsTeleport(Level& level, Coord move);
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

// Helper and scaffolding
string TransformString(string text, int operation);
void SetColor(int color);
void ShowColors(int colors);
void TopLeft(int rows);
string GhostMode(Ghost& ghost);

// ************
// MAIN PROGRAM
// ************

int main()
{
    Game game;
    Level level;
    Player player;
    Ghost redGhost, yellowGhost, orangeGhost, pinkGhost;
    Ghost ghosts[4] = { redGhost, yellowGhost, orangeGhost, pinkGhost };
    bool skip_turn = false;

    //ShowColors(500); // find colors to use

    // Set up variables and data
    SetUp(game, level, player, ghosts);

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

    return 0;
}

// level setup, display and management
void SetUp(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    // spawn ghost
    SpawnMonster(ghosts);
    
    // initialize array and set default player values
    CreateLevelScene(level, player, ghosts);

    // start time for ghost chase mode
    level.chase_time_start = chrono::high_resolution_clock::now();
}
void SpawnMonster(Ghost ghosts[])
{
    // spawn red ghost
    ghosts[0].ghost = 'R';
    ghosts[0].square_content_now = ' ';
    ghosts[0].square_content_prior = ghosts[0].square_content_now;
    ghosts[0].curr_pos = { ghosts[0].curr_pos.row = 0, ghosts[0].curr_pos.col = 0 };
    ghosts[0].spawn_pos = { ghosts[0].spawn_pos.row = 0, ghosts[0].spawn_pos.col = 0 };
    ghosts[0].prev_pos = { ghosts[0].prev_pos.row = 0, ghosts[0].prev_pos.col = 0 };
    ghosts[0].spawn_target = { ghosts[0].spawn_target.row = 10, ghosts[0].spawn_target.col = 24 }; // just above spawn area
    ghosts[0].roam_target = { ghosts[0].roam_target.row = 24, ghosts[0].roam_target.col = 45 }; // roams to top right hand side
    ghosts[0].chase_mod = { ghosts[0].chase_mod.row = 0, ghosts[0].chase_mod.col = 0 };
    ghosts[0].curr_direction = Direction::UP;
    ghosts[0].prev_direction = Direction::UP;
    ghosts[0].mode = Mode::CHASE;
    ghosts[0].run_first_move = true;
    ghosts[0].is_edible = false;
    ghosts[0].color_on = false;
    ghosts[0].color = 71;
    ghosts[0].rand = 5; // percent of time it will make random move choice
    ghosts[0].wait = 15;
    ghosts[0].skip_turn = false; // use this to slow down moster if edible
    ghosts[0].look_ahead = 5; // how far ahead the IA looks for player

    // spawn yellow ghost
    ghosts[1].ghost = 'Y';
    ghosts[1].square_content_now = ' ';
    ghosts[1].square_content_prior = ghosts[1].square_content_now;
    ghosts[1].curr_pos = { ghosts[1].curr_pos.row = 0, ghosts[1].curr_pos.col = 0 };
    ghosts[1].spawn_pos = { ghosts[1].spawn_pos.row = 0, ghosts[1].spawn_pos.col = 0 };
    ghosts[1].prev_pos = { ghosts[1].prev_pos.row = 0, ghosts[1].prev_pos.col = 0 };
    ghosts[1].spawn_target = { ghosts[1].spawn_target.row = 10, ghosts[1].spawn_target.col = 22 }; // just above spawn area
    ghosts[1].roam_target = { ghosts[1].roam_target.row = 24, ghosts[1].roam_target.col = 3 }; // roams to top right hand side
    ghosts[1].chase_mod = { ghosts[1].chase_mod.row = 0, ghosts[1].chase_mod.col = 3 };
    ghosts[1].curr_direction = Direction::UP;
    ghosts[1].prev_direction = Direction::UP;
    ghosts[1].mode = Mode::CHASE;
    ghosts[1].run_first_move = true;
    ghosts[1].is_edible = false;
    ghosts[1].color_on = false;
    ghosts[1].color = 367;
    ghosts[1].rand = 5; // percent of time it will make random move choice
    ghosts[1].wait = 30;
    ghosts[1].skip_turn = false; // use this to slow down moster if edible
    ghosts[1].look_ahead = 5; // how far ahead the IA looks for player

    // spawn blue ghost
    ghosts[2].ghost = 'O';
    ghosts[2].square_content_now = ' ';
    ghosts[2].square_content_prior = ghosts[2].square_content_now;
    ghosts[2].curr_pos = { ghosts[2].curr_pos.row = 0, ghosts[2].curr_pos.col = 0 };
    ghosts[2].spawn_pos = { ghosts[2].spawn_pos.row = 0, ghosts[2].spawn_pos.col = 0 };
    ghosts[2].prev_pos = { ghosts[2].prev_pos.row = 0, ghosts[2].prev_pos.col = 0 };
    ghosts[2].spawn_target = { ghosts[2].spawn_target.row = 10, ghosts[2].spawn_target.col = 22 }; // just above spawn area
    ghosts[2].roam_target = { ghosts[2].roam_target.row = -3, ghosts[2].roam_target.col = 2 }; // roams to top right hand side
    ghosts[2].chase_mod = { ghosts[2].chase_mod.row = 0, ghosts[2].chase_mod.col = -3 };
    ghosts[2].curr_direction = Direction::UP;
    ghosts[2].prev_direction = Direction::UP;
    ghosts[2].mode = Mode::CHASE;
    ghosts[2].run_first_move = true;
    ghosts[2].is_edible = false;
    ghosts[2].color_on = false;
    ghosts[2].color = 435;
    ghosts[2].rand = 5; // percent of time it will make random move choice
    ghosts[2].wait = 45;
    ghosts[2].skip_turn = false; // use this to slow down moster if edible
    ghosts[2].look_ahead = 5; // how far ahead the IA looks for player

    // spawn pink ghost
    ghosts[3].ghost = 'P';
    ghosts[3].square_content_now = ' ';
    ghosts[3].square_content_prior = ghosts[3].square_content_now;
    ghosts[3].curr_pos = { ghosts[3].curr_pos.row = 0, ghosts[3].curr_pos.col = 0 };
    ghosts[3].spawn_pos = { ghosts[3].spawn_pos.row = 0, ghosts[3].spawn_pos.col = 0 };
    ghosts[3].prev_pos = { ghosts[3].prev_pos.row = 0, ghosts[3].prev_pos.col = 0 };
    ghosts[3].spawn_target = { ghosts[3].spawn_target.row = 10, ghosts[3].spawn_target.col = 22 }; // just above spawn area
    ghosts[3].roam_target = { ghosts[3].roam_target.row = -3, ghosts[3].roam_target.col = 45 }; // roams to top right hand side
    ghosts[3].chase_mod = { ghosts[3].chase_mod.row = -3, ghosts[3].chase_mod.col = 0 };
    ghosts[3].curr_direction = Direction::UP;
    ghosts[3].prev_direction = Direction::UP;
    ghosts[3].mode = Mode::CHASE;
    ghosts[3].run_first_move = true;
    ghosts[3].is_edible = false;
    ghosts[3].color_on = false;
    ghosts[3].color = 479;
    ghosts[3].rand = 5; // percent of time it will make random move choice
    ghosts[3].wait = 60;
    ghosts[3].skip_turn = false; // use this to slow down moster if edible
    ghosts[3].look_ahead = 5; // how far ahead the IA looks for player
}
void SpawnMonster(Ghost ghosts[], Ghost ghost, bool playerDied)
{
    switch (ghost.ghost)
    {
    case 'R':
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
        ghosts[0].wait = (playerDied ? 15 : 6);
        ghosts[0].skip_turn = false; // use this to slow down moster if edible
        break;
    case 'Y':
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
        ghosts[1].wait = (playerDied ? 30 : 6);
        ghosts[1].skip_turn = false; // use this to slow down moster if edible
        break;
    case 'O':
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
        ghosts[2].wait = (playerDied ? 45 : 6);
        ghosts[2].skip_turn = false; // use this to slow down moster if edible
        break;
    case 'P':
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
        ghosts[3].wait = (playerDied ? 60 : 6);
        ghosts[3].skip_turn = false; // use this to slow down moster if edible
        break;
    }

    
}
void CreateLevelScene(Level& level, Player& player, Ghost ghosts[])
{
    // create string with the level - this makes it easier to convert a string diagram to a level array
    // must be of the level dimensions 17 rows by 23 cols
    string map;
    /*map = "+----------------------+----------------------+";
    map += "|%o...................%|%...................o%|";
    map += "|%.%+-----+%.%+----+%.%|%.%+----+%.%+-----+%.%|";
    map += "|%.%#######%.%######%.%|%.%######%.%#######%.%|";
    map += "|%.%+-----+%.%+----+%.%|%.%+----+%.%+-----+%.%|";
    map += "|%...........................................%|";
    map += "|%.%+-----+%.%|%.%+---------+%.%|%.%+-----+%.%|";
    map += "|%.%+-----+%.%|%.%+---------+%.%|%.%+-----+%.%|";
    map += "|%...........%|%......%|%......%|%...........%|";
    map += "|%.%+-----+%.%+----+%.%+%.%+----+%.%+-----+%.%|";
    map += "|%.%#######%.%|%...............%|%.%#######%.%|";
    map += "|%.%+-----+%.%|%.%+--+$$$+--+%.%|%.%+-----+%.%|";
    map += "T ...........%|%.%|%O%R%Y%P%|%.%|%........... T";
    map += "|%.%+-----+%.%|%.%+---------+%.%|%.%+-----+%.%|";
    map += "|%.%#######%.%|%...............%|%.%#######%.%|";
    map += "|%.%#######%.%|%.%+---------+%.%|%.%#######%.%|";
    map += "|%.%+-----+%.%|%.%+---------+%.%|%.%+-----+%.%|";
    map += "|%.....................S.....................%|";
    map += "|%.%+-----+%.%+----+%.%|%.%+----+%.%+-----+%.%|";
    map += "|%.%#######%.%######%.%|%.%######%.%#######%.%|";
    map += "|%.%+-----+%.%+----+%.%|%.%+----+%.%+-----+%.%|";
    map += "|%o...................%|%...................o%|";
    map += "+----------------------+----------------------+";*/

    map += "+----------------------+----------------------+";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|...o.....................................o...|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|................+--+$$$+--+..................|";
    map += "T................|%O%R%Y%P%|..................T";
    map += "|................+---------+..................|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|......................S......................|";
    map += "|...o.....................................o...|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "|.............................................|";
    map += "+----------------------+----------------------+";

    // '|', '+' = wall; '%' = blank spot with collision; 'T' = portal; 'S' = player starting pos; 'M' = monsters; $ = one way exit only

    int t_row = 0, t_col = 0;

    // iterate through the string
    for (string::size_type i = 0; i < map.size(); i++)
    {
        // add to level map array
        level.map[t_row][t_col] = map[i];

        // count pellets that determine end game condition
        if (map[i] == Level::chr_pellet || map[i] == Level::chr_powerup)
            level.total_pellets++;

        // Set player start to position 'S'
        if (map[i] == 'S')
        {
            player.spawn_pos.row = player.curr_pos.row = player.prev_pos.row = t_row;
            player.spawn_pos.col = player.curr_pos.col = player.prev_pos.col = t_col;
            level.map[t_row][t_col] = Level::chr_pellet;
            level.total_pellets++;
        }

        // Set Ghost start position
        for (int g = 0; g < 4; g++) // loop through ghosts
        {
            if (map[i] == ghosts[g].ghost)
            {
                ghosts[g].spawn_pos.row = ghosts[g].curr_pos.row = ghosts[g].prev_pos.row = t_row;
                ghosts[g].spawn_pos.col = ghosts[g].curr_pos.col = ghosts[g].prev_pos.col = t_col;
                ghosts[g].spawn_target.row = 8;
                ghosts[g].spawn_target.col = 25;
            }
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
    
    for (int r = 0; r < level.rows; r++)
    {
        for (int c = 0; c < level.cols; c++)
        {
            switch(level.map[r][c])
            {
            case 'R':
                level.map[r][c] = ghosts[0].square_content_prior;
            case 'Y':
                level.map[r][c] = ghosts[1].square_content_prior;
            case 'O':
                level.map[r][c] = ghosts[2].square_content_prior;
            case 'P':
                level.map[r][c] = ghosts[3].square_content_prior;
            }
        }
    }
    
    // place cursor on top left of console
    TopLeft(level.rows + 5); // + 5 for title and status

    // remove player from map at last position if they are different
    if (player.prev_pos.row != player.curr_pos.row || player.prev_pos.col != player.curr_pos.col)
        level.map[player.prev_pos.row][player.prev_pos.col] = ' ';

    // player in tunnel
    if (player.curr_pos.row == 12 && player.curr_pos.col == 0) {
        level.map[player.curr_pos.row][player.curr_pos.col] = 'T';
        player.curr_pos.col = 46;
    }
    else if (player.curr_pos.row == 12 && player.curr_pos.col == 46) {
        level.map[player.curr_pos.row][player.curr_pos.col] = 'T';
        player.curr_pos.col = 0;
    }
    
    for (int g = 0; g < 4; g++) // loop through ghots
    {
        // remove ghosts from map at last position if they are different
        if (ghosts[g].prev_pos.row != ghosts[g].curr_pos.row || ghosts[g].prev_pos.col != ghosts[g].curr_pos.col)
            level.map[ghosts[g].prev_pos.row][ghosts[g].prev_pos.col] = ' ';
        
        // ghost in tunnel
        if (!ghosts[g].skip_turn)
        {
            if (ghosts[g].curr_pos.row == 12 && ghosts[g].curr_pos.col == 0) {
                level.map[ghosts[g].curr_pos.row][ghosts[g].curr_pos.col] = 'T';
                ghosts[g].curr_pos.col = 46;
            }
            else if (ghosts[g].curr_pos.row == 12 && ghosts[g].curr_pos.col == 46) {
                level.map[ghosts[g].curr_pos.row][ghosts[g].curr_pos.col] = 'T';
                ghosts[g].curr_pos.col = 0;
            }
        }
        
    }
   
    // Level Title
    cout << endl << "                 PACMAN: " << TransformString(level.title, 0) << endl << endl;

    // draw current level map
    for (int r = 0; r < level.rows; r++)
    {
        for (int c = 0; c < level.cols; c++)
        {
            // position player
            if (r == player.curr_pos.row && c == player.curr_pos.col)
                level.map[r][c] = player.player;

            for (int g = 0; g < 4; g++) // loop through ghots
            {
                // position ghost
                if (r == ghosts[g].curr_pos.row && c == ghosts[g].curr_pos.col)
                    level.map[r][c] = ghosts[g].ghost;

                // if the ghost moved from the last position
                if (ghosts[g].prev_pos.row != ghosts[g].curr_pos.row || ghosts[g].prev_pos.col != ghosts[g].curr_pos.col)
                {
                    // put back the content of the square the ghost was last at 
                    if (r == ghosts[g].prev_pos.row && c == ghosts[g].prev_pos.col)
                        level.map[r][c] = ghosts[g].square_content_prior;
                }
            }

            // set color of map content at this position
            switch (level.map[r][c])
            {
            case '%': // invisible wall
            case 'T': // portal
            case ' ': // empty space
            case '$': // one way door for ghost spawn area
                SetColor(0); // black on black = not visible
                break;
            case '.': // pellet
                SetColor(7); // gray for bullets
                break;
            case 'o': // power up
                SetColor(7); // white for power ups
                break;
            case 'P': // blue ghost
                if (ghosts[3].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[3].color_on ? SetColor(game.ghost_color_on) : SetColor(game.ghost_color_off);
                    ghosts[3].color_on = !ghosts[3].color_on;
                }
                else { // solid color
                    SetColor(ghosts[3].color);
                }
                break;
            case 'Y': // yellow ghost
                if (ghosts[1].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[1].color_on ? SetColor(game.ghost_color_on) : SetColor(game.ghost_color_off);
                    ghosts[1].color_on = !ghosts[1].color_on;
                } else { // solid color
                    SetColor(ghosts[1].color);
                }
                break;
            case 'O': // green ghost
                if (ghosts[2].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[2].color_on ? SetColor(game.ghost_color_on) : SetColor(game.ghost_color_off);
                    ghosts[2].color_on = !ghosts[2].color_on;
                }
                else { // solid color
                    SetColor(ghosts[2].color);
                }
                break;
            case 'R': // red ghost
                if (ghosts[0].is_edible) { // flashing effect - signals edible sate of ghost
                    ghosts[0].color_on ? SetColor(game.ghost_color_on) : SetColor(game.ghost_color_off);
                    ghosts[0].color_on = !ghosts[0].color_on;
                } else { // solid color
                    SetColor(ghosts[0].color);
                }
                break;
            case Player::player: // player
                SetColor(14);
                break;
            default:
                SetColor(392); // gray bg on gray text for all other chars making them walls essentially
                break;
            }

            // print char
            cout << level.map[r][c];

            // set color back to default
            SetColor(7);
        }
        // end of row ad line feed
        cout << endl;
    }
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
    cout << "   ";
    cout << "\r";
}

// Player movement
void MovePlayer(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    player.prev_pos.row = player.curr_pos.row;
    player.prev_pos.col = player.curr_pos.col;
    switch (player.curr_direction)
    {
    case Direction::UP: //up
        if (PlayerCanMove(level, player.curr_pos.row - 1, player.curr_pos.col)) {
            player.curr_pos.row--;
            SetPlayerState(game, level, player, ghosts);
            player.prev_direction = Direction::UP;
        }
        else {
            KeepMovePlayer(game, level, player, ghosts);
        }
        break;
    case Direction::RIGHT: // right
        if (PlayerCanMove(level, player.curr_pos.row, player.curr_pos.col + 1)) {
            player.curr_pos.col++;
            SetPlayerState(game, level, player, ghosts);
            player.prev_direction = Direction::RIGHT;
        }
        else {
            KeepMovePlayer(game, level, player, ghosts);
        }
        break;
    case Direction::DOWN: // down
        if (PlayerCanMove(level, player.curr_pos.row + 1, player.curr_pos.col)) {
            player.curr_pos.row++;
            SetPlayerState(game, level, player, ghosts);
            player.prev_direction = Direction::DOWN;
        }
        else {
            KeepMovePlayer(game, level, player, ghosts);
        }
        break;
    case Direction::LEFT: // left
        if (PlayerCanMove(level, player.curr_pos.row, player.curr_pos.col - 1)) {
            player.curr_pos.col--;
            SetPlayerState(game, level, player, ghosts);
            player.prev_direction = Direction::LEFT;
        }
        else {
            KeepMovePlayer(game, level, player, ghosts);
        }
        break;
    default:
        KeepMovePlayer(game, level, player, ghosts);
        break;
    }
    for (int g = 0; g < 4; g++)
    {
        if (CheckCollision(player, ghosts[g])) {
            ghosts[g].prev_pos.row = player.curr_pos.row;
            ghosts[g].prev_pos.col = player.curr_pos.col;
        }
    }
    
}
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
void KeepMovePlayer(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    player.prev_pos.row = player.curr_pos.row;
    player.prev_pos.col = player.curr_pos.col;
    switch (player.prev_direction)
    {
    case Direction::UP: //up
        if (PlayerCanMove(level, player.curr_pos.row - 1, player.curr_pos.col)) {
            player.curr_pos.row--;
            SetPlayerState(game, level, player, ghosts);
        }

        break;
    case Direction::RIGHT: // right
        if (PlayerCanMove(level, player.curr_pos.row, player.curr_pos.col + 1)) {
            player.curr_pos.col++;
            SetPlayerState(game, level, player, ghosts);
        }

        break;
    case Direction::DOWN: // down
        if (PlayerCanMove(level, player.curr_pos.row + 1, player.curr_pos.col)) {
            player.curr_pos.row++;
            SetPlayerState(game, level, player, ghosts);
        }

        break;
    case Direction::LEFT: // left
        if (PlayerCanMove(level, player.curr_pos.row, player.curr_pos.col - 1)) {
            player.curr_pos.col--;
            SetPlayerState(game, level, player, ghosts);
        }
        break;
    }
}
bool PlayerCanMove(Level& level, int newPRow, int newPCol)
{
    char levelObj = level.map[newPRow][newPCol];
    switch (levelObj)
    {
    case ' ':
    case 'T':
    case '.':
    case 'o':
    case 'R':
    case 'Y':
    case 'O':
    case 'P':
        return true;
    }
    return false;
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
bool NotWall(Level& level, Coord move, Direction direction)
{
    switch (level.map[move.row][move.col])
    {
    case ' ':
    case 'T':
    case '.':
    case 'o':
    case 'R':
    case 'Y':
    case 'O':
    case 'P':
    case 'C':
        return true;
    case '$': // one way move to exit ghost spawn area
        return (direction != Direction::DOWN ? true : false);

    }
    return false;
}
bool IsTeleport(Level& level, Coord move)
{
    if (level.tp_1.row == move.row && level.tp_1.col == move.col)
        return true;
    if (level.tp_2.row == move.row && level.tp_2.col == move.col)
        return true;
    return false;
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
    if (ghost.curr_pos.row == level.tp_1.row && ghost.curr_pos.col == level.tp_1.col)
    {
        // stop
        int test = 1;
    }

    if (ghost.curr_pos.row == level.tp_2.row && ghost.curr_pos.col == level.tp_2.col)
    {
        // stop
        int test = 1;
    }

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
        int randomNumber = rand() % 4; //generate random number to select from the 4 possible options
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
        //IsTeleport(level, { ghost.curr_pos.row, ghost.curr_pos.col+1 }) ? GhostTeleport(level, ghost, { ghost.curr_pos.row, ghost.curr_pos.col + 1 }, bestMove).col : ghost.curr_pos.col++;
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
        //IsTeleport(level, { ghost.curr_pos.row, ghost.curr_pos.col + 1 }) ? GhostTeleport(level, ghost, { ghost.curr_pos.row, ghost.curr_pos.col + 1 }, bestMove).col : ghost.curr_pos.col--;
        ghost.curr_pos.col--;
        ghost.curr_direction = Direction::LEFT;
        break;
    default:
        break;
    }
    // if the sqaure the ghost moved into is the player
    ghost.square_content_now == 'C' ? ghost.square_content_now = ' ' : ghost.square_content_now;

    // if the mosnter is over the player save a blank space to buffer
    CheckCollision(player, ghost) ? ghost.square_content_now = ' ' : ghost.square_content_now;

}
char GetSquareContentNow(Level& level, Ghost ghosts[], Coord square)
{
    switch (level.map[square.row][square.col])
    {
        case '.':
        case 'o':
        case ' ':
            return(level.map[square.row][square.col]);
        case 'R':
            return ghosts[0].square_content_now;
        case 'Y':
            return ghosts[1].square_content_now;
        case 'O':
            return ghosts[2].square_content_now;
        case 'P':
            return ghosts[3].square_content_now;
        default:
            return(level.map[square.row][square.col]);
    }
}

// Player/Ghost Events and Status
void SetPlayerState(Game& game, Level& level, Player& player, Ghost ghosts[])
{
    char levelObj = level.map[player.curr_pos.row][player.curr_pos.col];
    switch (levelObj)
    {
    case '.': // eat pellet
        level.eaten_pellets++;
        break;
    case 'o': // eat power up
        level.level_mode = Mode::RUN;
        level.run_time_start = chrono::high_resolution_clock::now();
        level.eaten_pellets++;
        for (int g = 0; g < 4; g++) // loop ghosts
        {
            ghosts[g].mode != Mode::SPAWN ? ghosts[g].is_edible = true : ghosts[g].is_edible = false; // ghost only edible on power up if out of spwan area
            ghosts[g].is_edible ? ghosts[g].mode = Mode::RUN : ghosts[g].mode = ghosts[g].mode;
            ghosts[g].run_first_move = true;
        }
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
                if (ghosts[g].is_edible)
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
                if (ghosts[g].square_content_now == Level::chr_pellet || ghosts[g].square_content_now == Level::chr_powerup)
                    level.eaten_pellets++;
                level.eaten_ghots++; // increment ghosts eaten
                game.gobble_pause = true; // set small pause after eating ghost
                SpawnMonster(ghosts, ghosts[g], false); // reset ghost to spawn area whihc resets mode to spawn
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
                level.map[ghosts[g].curr_pos.row][ghosts[g].curr_pos.col] = ' ';
                // respawn ghost
                SpawnMonster(ghosts, ghosts[g], true);
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
    level.eaten_pellets == level.total_pellets - 1 ? level.is_complete = true : level.is_complete = false; // if all pellets are eaten game ends - 339 in this map
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
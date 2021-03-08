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
struct Monster {
    static constexpr char monster = 'R';
    char square_content_now = ' ';
    char square_content_prior = square_content_now;
    Coord curr_pos = { curr_pos.row = 0, curr_pos.col = 0 };
    Coord spawn_pos = { spawn_pos.row = 0, spawn_pos.col = 0 };
    Coord prev_pos = { prev_pos.row = 0, prev_pos.col = 0 };
    Coord spawn_target = { spawn_target.row = 10, spawn_target.col = 22 }; // just above spawn area
    Coord roam_target = { roam_target.row = -2, roam_target.col = 45 }; // roams to top right hand side
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
    char map[23][47]{};
    int rows = 23;
    int cols = 47;
    Coord tp_1 = { tp_1.row = 0, tp_1.col = 0 };
    Coord tp_2 = { tp_2.row = 0, tp_2.col = 0 };
    bool tp_row = true;
    int total_pellets = 0; // totala level pellets
    int eaten_pellets = 0; // pellets consumed
    int eaten_ghots = 0; // pellets consumed
    int edible_ghost_duration = 10; // seconds
    char pelletChar = '.';
    char powerUpChar = 'o';
    bool is_complete = false;
    int points_pellet = 10;
    int points_ghost = 250;
    int all_ghost_bonus = 2600;
    string title = "Scene 1";
    string author = "Uris D";
    static constexpr char chr_powerup = 'o';
    static constexpr char chr_pellet = '.';
    int roam_count = 4;
    int roam_for = 10; // seconds
    int chase_for = 15; // seconds
    high_resolution_clock::time_point time_start = chrono::high_resolution_clock::now();
    high_resolution_clock::time_point time_now = chrono::high_resolution_clock::now();
    duration<double> roam_duration = duration_cast<duration<double>>(time_start - time_now);
    duration<double> chase_duration = duration_cast<duration<double>>(time_start - time_now);
};
struct Game {
    bool game_over = false;
    bool gobble_pause = false;
    bool player_beat_pause = false;
    int gobble_delay = 500; // milliseconds
    int player_beat_delay = 1000; // milliseconds
    int refresh_delay = 15; // milliseconds
    high_resolution_clock::time_point g_t_start = chrono::high_resolution_clock::now(); // start time of high precision timer
    high_resolution_clock::time_point g_t_now = chrono::high_resolution_clock::now(); // start time of high precision timer
    duration<double> time_span = duration_cast<duration<double>>(g_t_now - g_t_start); // will hold time difference
};

// Functions -->

// level setup, display and management
void SetUp(Game& game, Level& level, Player& player, Monster& monster);
void CreateLevelScene(Level& level, Player& player, Monster& monster);
void DrawLevel(Level& level, Player& player, Monster& monster);
void StatusBar(Game& game, Level& level, Player& player, Monster& monster);
void SpawnMonster(Monster& monster);
void SpawnPlayer(Player& player);

// Player movement
void GetPlayerDirection(Game& game, Level& level, Player& player);
void MovePlayer(Game& game, Level& level, Player& player, Monster& monster);
void KeepMovePlayer(Game& game, Level& level, Player& player, Monster& monster);
bool PlayerCanMove(Level& level, int newPRow, int newPCol);

// Ghost movement
int MoveMonster(Game& game, Level& level, Player& player, Monster& monster);
void DoMonsterMove(Level& level, Player& player, Monster& monster, Direction bestMove);
int GetBestMove(Level& level, Player& player, Monster& monster, Coord move, Direction curr_direction, int depth, bool isGhost);
bool NotWall(Level& level, Coord move, Direction direction);
bool CanMove(Level& level, Monster& monster, Coord move, Direction direction, Direction curr_direction);
Coord NewMove(Level& level, Monster& monster, Coord move, Direction direction);
bool IsTeleport(Level& level, Coord move);
Direction RandomGhostMove(Level& level, Monster& monster);

// Player/Monster Events and Status
void PlayerMonsterCollision(Game& game, Level& level, Player& player, Monster& monster);
bool CheckCollision(Player& player, Monster& monster);
void SetPlayerState(Game& game, Level& level, Player& player, Monster& monster);
void SetSuperPlayer(Game& game, Level& level, Player& player, Monster& monster);
void SetGhostMode(Level& level, Player& player, Monster& monster);

// Game management
void CheckLevelComplete(Level& level);
void RefreshDelay(Game& game, Level& level);
void PlayerMonsterCollisionDelay(Game& game, Level& level);

// Helper and scaffolding
string TransformString(string text, int operation);
void SetColor(int color);
void ShowColors(int colors);
void TopLeft(int rows);
string GhostMode(Monster& monster);

// ************
// MAIN PROGRAM
// ************

int main()
{
    Game game;
    Level level;
    Player player;
    Monster monsterRed;
    bool skip_turn = false;

    //ShowColors(500); // find colors to use

    // Set up variables and data
    SetUp(game, level, player, monsterRed);

    // draw initial level state
    DrawLevel(level, player, monsterRed);

    // start a thread to get input while the main program executes
    thread inputThread(GetPlayerDirection, ref(game), ref(level), ref(player)); // new the ref() wrapper to pass by ref into thread

    do
    {
        // move the player
        MovePlayer(game, level, player, monsterRed);

        // process any player / monster collision
        PlayerMonsterCollision(game, level, player, monsterRed);

        // set the player to normal or super player if he powers up
        SetSuperPlayer(game, level, player, monsterRed);

        // set ghost chase modes as approprite
        SetGhostMode(level, player, monsterRed);

        // move the monster
        MoveMonster(game, level, player, monsterRed);

        // process any player / monster collision again
        PlayerMonsterCollision(game, level, player, monsterRed);

        // delay render if there's a collision
        PlayerMonsterCollisionDelay(game, level);

        // Draw the level current state
        DrawLevel(level, player, monsterRed);

        // display stats and number lives
        StatusBar(game, level, player, monsterRed);

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
void SetUp(Game& game, Level& level, Player& player, Monster& monster)
{
    // spawn monster
    SpawnMonster(monster);

    // start time for ghost roam mode - every 15 seconds after chase mode
    level.time_start = chrono::high_resolution_clock::now();

    // initialize array and set default player values
    CreateLevelScene(level, player, monster);
}
void CreateLevelScene(Level& level, Player& player, Monster& monster)
{
    // create string with the level - this makes it easier to convert a string diagram to a level array
    // must be of the level dimensions 17 rows by 23 cols
    string map;
    map = "+----------------------+----------------------+";
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
    map += "T ...........%|%.%|%B%R%Y%G%|%.%|%........... T";
    map += "|%.%+-----+%.%|%.%+---------+%.%|%.%+-----+%.%|";
    map += "|%.%#######%.%|%...............%|%.%#######%.%|";
    map += "|%.%#######%.%|%.%+---------+%.%|%.%#######%.%|";
    map += "|%.%+-----+%.%|%.%+---------+%.%|%.%+-----+%.%|";
    map += "|%.....................S.....................%|";
    map += "|%.%+-----+%.%+----+%.%|%.%+----+%.%+-----+%.%|";
    map += "|%.%#######%.%######%.%|%.%######%.%#######%.%|";
    map += "|%.%+-----+%.%+----+%.%|%.%+----+%.%+-----+%.%|";
    map += "|%o...................%|%...................o%|";
    map += "+----------------------+----------------------+";

    // '|', '+' = wall; '%' = blank spot with collision; 'T' = portal; 'S' = player starting pos; 'M' = monsters; $ = one way exit only

    int t_row = 0, t_col = 0;

    // iterate through the string
    for (string::size_type i = 0; i < map.size(); i++)
    {
        // add to level map array
        level.map[t_row][t_col] = map[i];

        // count pellets that determine end game condition
        if (map[i] == level.pelletChar || map[i] == level.powerUpChar)
            level.total_pellets++;

        // Set player start to position 'S'
        if (map[i] == 'S')
        {
            player.spawn_pos.row = player.curr_pos.row = player.prev_pos.row = t_row;
            player.spawn_pos.col = player.curr_pos.col = player.prev_pos.col = t_col;
            level.map[t_row][t_col] = level.pelletChar;
            level.total_pellets++;
        }

        // Set Monster start position
        if (map[i] == monster.monster)
        {
            monster.spawn_pos.row = monster.curr_pos.row = monster.prev_pos.row = t_row;
            monster.spawn_pos.col = monster.curr_pos.col = monster.prev_pos.col = t_col;
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
void SpawnMonster(Monster& monster)
{
    monster.square_content_now = ' ';
    monster.square_content_prior = monster.square_content_now;
    monster.curr_pos.row = monster.spawn_pos.row;
    monster.curr_pos.col = monster.spawn_pos.col;
    monster.curr_direction = Direction::UP;
    monster.prev_direction = Direction::UP;
    monster.mode = Mode::SPAWN;
    monster.run_first_move = true;
    monster.is_edible = false;
    monster.color_on = false;
    monster.color = 455;
    monster.wait = 20; // wait before leaving spawn area
    monster.rand = 5; // percent of time it will make random move choice
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
void DrawLevel(Level& level, Player& player, Monster& monster)
{
    // place cursor on top left of console
    TopLeft(level.rows + 5); // + 5 for title and status

    // remove player and monster from map at last position if they are different
    if (player.prev_pos.row != player.curr_pos.row || player.prev_pos.col != player.curr_pos.col)
        level.map[player.prev_pos.row][player.prev_pos.col] = ' ';
    if (monster.prev_pos.row != monster.curr_pos.row || monster.prev_pos.col != monster.curr_pos.col)
        level.map[monster.prev_pos.row][monster.prev_pos.col] = ' ';

    // player in tunnel
    if (player.curr_pos.row == 12 && player.curr_pos.col == 0) {
        level.map[player.curr_pos.row][player.curr_pos.col] = 'T';
        player.curr_pos.col = 46;
    }
    else if (player.curr_pos.row == 12 && player.curr_pos.col == 46) {
        level.map[player.curr_pos.row][player.curr_pos.col] = 'T';
        player.curr_pos.col = 0;
    }

    // monster in tunnel
    if (monster.curr_pos.row == 12 && monster.curr_pos.col == 0) {
        level.map[monster.curr_pos.row][monster.curr_pos.col] = 'T';
        monster.curr_pos.col = 46;
    }
    else if (monster.curr_pos.row == 12 && monster.curr_pos.col == 46) {
        level.map[monster.curr_pos.row][monster.curr_pos.col] = 'T';
        monster.curr_pos.col = 0;
    }

    // Level Title
    cout << endl << "   PACMAN: " << TransformString(level.title, 0) << " by " << TransformString(level.author, 0) << endl << endl;

    // draw current level map
    for (int r = 0; r < level.rows; r++)
    {
        for (int c = 0; c < level.cols; c++)
        {
            // position player
            if (r == player.curr_pos.row && c == player.curr_pos.col)
                level.map[r][c] = player.player;

            // position moster
            if (r == monster.curr_pos.row && c == monster.curr_pos.col)
                level.map[r][c] = monster.monster;

            // if the monster moved from the last position
            if (monster.prev_pos.row != monster.curr_pos.row || monster.prev_pos.col != monster.curr_pos.col)
            {
                // put back the content of the square the monster was last at 
                if (r == monster.prev_pos.row && c == monster.prev_pos.col)
                    level.map[r][c] = monster.square_content_prior;
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
            case 'B': // blue ghost
                SetColor(31);
                break;
            case 'Y': // yellow ghost
                SetColor(352);
                break;
            case 'G': // green ghost
                SetColor(432); // cyan for monsters
                break;
            case 'R': // red ghost
                if (monster.is_edible) // flashing effect - signals edible sate of ghost
                {
                    monster.color_on ? SetColor(12) : SetColor(monster.color);
                    monster.color_on = !monster.color_on;
                }
                else // solid color
                {
                    SetColor(monster.color);
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
void StatusBar(Game& game, Level& level, Player& player, Monster& monster)
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
    cout << " LIVES:";
    SetColor(14);
    cout << (player.lives > 0 ? lives : "-");
    SetColor(7);
    cout << " SCORE:";
    SetColor(14);
    cout << setfill('0') << setw(8) << player.score;
    SetColor(7);
    cout << " GHOSTS:";
    SetColor(14);
    cout << (level.eaten_ghots >= 4 ? "*" : "") << "x" << level.eaten_ghots << (level.eaten_ghots >= 4 ? "*" : "");
    SetColor(7);
    cout << " M:";
    SetColor(14);
    cout << GhostMode(monster) << " ";
    cout << (monster.run_first_move ? "T" : "F");
    cout << "   ";
    SetColor(7);
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
void MovePlayer(Game& game, Level& level, Player& player, Monster& monster)
{
    player.prev_pos.row = player.curr_pos.row;
    player.prev_pos.col = player.curr_pos.col;
    switch (player.curr_direction)
    {
    case Direction::UP: //up
        if (PlayerCanMove(level, player.curr_pos.row - 1, player.curr_pos.col)) {
            player.curr_pos.row--;
            SetPlayerState(game, level, player, monster);
            player.prev_direction = Direction::UP;
        }
        else {
            KeepMovePlayer(game, level, player, monster);
        }
        break;
    case Direction::RIGHT: // right
        if (PlayerCanMove(level, player.curr_pos.row, player.curr_pos.col + 1)) {
            player.curr_pos.col++;
            SetPlayerState(game, level, player, monster);
            player.prev_direction = Direction::RIGHT;
        }
        else {
            KeepMovePlayer(game, level, player, monster);
        }
        break;
    case Direction::DOWN: // down
        if (PlayerCanMove(level, player.curr_pos.row + 1, player.curr_pos.col)) {
            player.curr_pos.row++;
            SetPlayerState(game, level, player, monster);
            player.prev_direction = Direction::DOWN;
        }
        else {
            KeepMovePlayer(game, level, player, monster);
        }
        break;
    case Direction::LEFT: // left
        if (PlayerCanMove(level, player.curr_pos.row, player.curr_pos.col - 1)) {
            player.curr_pos.col--;
            SetPlayerState(game, level, player, monster);
            player.prev_direction = Direction::LEFT;
        }
        else {
            KeepMovePlayer(game, level, player, monster);
        }
        break;
    default:
        KeepMovePlayer(game, level, player, monster);
        break;
    }
    if (CheckCollision(player, monster)) {
        monster.prev_pos.row = player.curr_pos.row;
        monster.prev_pos.col = player.curr_pos.col;
    }
}
void KeepMovePlayer(Game& game, Level& level, Player& player, Monster& monster)
{
    player.prev_pos.row = player.curr_pos.row;
    player.prev_pos.col = player.curr_pos.col;
    switch (player.prev_direction)
    {
    case Direction::UP: //up
        if (PlayerCanMove(level, player.curr_pos.row - 1, player.curr_pos.col)) {
            player.curr_pos.row--;
            SetPlayerState(game, level, player, monster);
        }

        break;
    case Direction::RIGHT: // right
        if (PlayerCanMove(level, player.curr_pos.row, player.curr_pos.col + 1)) {
            player.curr_pos.col++;
            SetPlayerState(game, level, player, monster);
        }

        break;
    case Direction::DOWN: // down
        if (PlayerCanMove(level, player.curr_pos.row + 1, player.curr_pos.col)) {
            player.curr_pos.row++;
            SetPlayerState(game, level, player, monster);
        }

        break;
    case Direction::LEFT: // left
        if (PlayerCanMove(level, player.curr_pos.row, player.curr_pos.col - 1)) {
            player.curr_pos.col--;
            SetPlayerState(game, level, player, monster);
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
    case 'G':
    case 'B':
    case 'Y':
        return true;
    }
    return false;
}

// Monster movement
int GetBestMove(Level& level, Player& player, Monster& monster, Coord move, Direction curr_direction, int depth, bool isGhost)
{
    // calculate based on the ghost mode

    switch (monster.mode)
    {
    case Mode::CHASE: // redude distance to player
        if ((abs(monster.curr_pos.col - player.curr_pos.col) + abs(monster.curr_pos.row - player.curr_pos.row)) == 0) { // if player and ghost collide return 0 + depth as score
            return 0 + depth; // add depth to get fastest path
        }
        if (depth == monster.look_ahead) { // if depth X return the distance score, increase this to make the ghost look forward more
            return (abs(monster.curr_pos.col - player.curr_pos.col) + abs(monster.curr_pos.row - player.curr_pos.row) + depth); // add depth to get fastest path
        }
        break;
    case Mode::RUN: // will be random in future - for now just get as far away from playet as possible
        return abs(monster.curr_pos.col - player.curr_pos.col) + abs(monster.curr_pos.row - player.curr_pos.row);
        break;
    case Mode::ROAM: // reduce distance to the ghost's roam target
        if (depth == monster.look_ahead) {
            return abs(monster.curr_pos.col - monster.roam_target.col) + abs(monster.curr_pos.row - monster.roam_target.row);
        }
        break;
    case Mode::SPAWN: // target is above the exit area
        int temp = abs(monster.curr_pos.col - monster.spawn_target.col) + abs(monster.curr_pos.row - monster.spawn_target.row);
        if (temp == 0)
            monster.mode = Mode::CHASE;
        return temp;
        break;
    }

    if (isGhost) {
        // if its the ghost move we want the lowest possible distance to player
        // right now we won't play the player move unless this doesn't work well.
        int score = 1000, bestScore = monster.mode == Mode::RUN ? -1000 : 1000;
        Coord nextMove;
        Direction new_direction = Direction::NONE;

        for (int i = 0; i <= 3; i++) // cycle through each next possible move up, down, left, right
        {
            new_direction = static_cast<Direction>(i); // cast index to direction up, down, left, right
            if (CanMove(level, monster, move, new_direction, curr_direction)) // check if the direction is valid i.e not wall or reverse
            {
                // set new coords of ghost
                nextMove = NewMove(level, monster, move, new_direction);
                monster.curr_pos.row = nextMove.row; monster.curr_pos.col = nextMove.col;

                // calculate distance score based on new coords
                int score = GetBestMove(level, player, monster, nextMove, new_direction, depth + 1, true); // get the minimax score for the move (recurssive)

                // revert ghost coords back
                monster.curr_pos.row = move.row; monster.curr_pos.col = move.col;

                // if the score of is better than the current bestscore set the score to be the new best score (min to get closer, max when running away)
                if (monster.mode == Mode::RUN)
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
Coord NewMove(Level& level, Monster& monster, Coord move, Direction direction)
{
    // based on direction, calc new coord of the move and return it
    // if moving from a teleport to a teleport make the jump based on whether the teleport is horizontal or vertical
    Coord newCoord;
    switch (direction)
    {
    case Direction::UP:
        (level.tp_row && move.row == level.tp_1.row && move.col == level.tp_1.col) ? newCoord.row = level.tp_2.row : newCoord.row = move.row - 1;
        newCoord.col = move.col;
        break;
    case  Direction::DOWN:
        (level.tp_row && move.row == level.tp_2.row && move.col == level.tp_2.col) ? newCoord.row = level.tp_1.row : newCoord.row = move.row += 1;
        newCoord.col = move.col;
        break;
    case Direction::LEFT:
        (!level.tp_row && move.row == level.tp_1.row && move.col == level.tp_1.col) ? newCoord.col = level.tp_2.col : newCoord.col = move.col - 1;
        newCoord.row = move.row;
        break;
    case  Direction::RIGHT:
        (!level.tp_row && move.row == level.tp_2.row && move.col == level.tp_2.col) ? newCoord.col = level.tp_1.col : newCoord.col = move.col + 1;
        newCoord.row = move.row;
        break;
    }
    return newCoord;
}
bool CanMove(Level& level, Monster& monster, Coord move, Direction direction, Direction curr_direction)
{
    // can only do one reverse move if on the run otherwsie no reverse moves
    switch (direction)
    {
    case  Direction::UP:
        if (curr_direction == Direction::DOWN)
        {
            if (monster.mode == Mode::RUN && monster.run_first_move)
                monster.run_first_move = false;
            else
                return false;
        }
        break;
    case  Direction::DOWN:
        if (curr_direction == Direction::UP)
        {
            if (monster.mode == Mode::RUN && monster.run_first_move)
                monster.run_first_move = false;
            else
                return false;
        }
        break;
    case  Direction::LEFT:
        if (curr_direction == Direction::RIGHT)
        {
            if (monster.mode == Mode::RUN && monster.run_first_move)
                monster.run_first_move = false;
            else
                return false;
        }
        break;
    case  Direction::RIGHT:
        if (curr_direction == Direction::LEFT)
        {
            if (monster.mode == Mode::RUN && monster.run_first_move)
                monster.run_first_move = false;
            else
                return false;
        }
        break;
    }

    // see if the next move is viable
    switch (direction)
    {
    case  Direction::UP:
        return (NotWall(level, NewMove(level, monster, move, Direction::UP), Direction::UP) ? true : false);
        break;
    case  Direction::DOWN:
        return (NotWall(level, NewMove(level, monster, move, Direction::DOWN), Direction::DOWN) ? true : false);
        break;
    case  Direction::LEFT:
        return (NotWall(level, NewMove(level, monster, move, Direction::LEFT), Direction::LEFT) ? true : false);
        break;
    case  Direction::RIGHT:
        return (NotWall(level, NewMove(level, monster, move, Direction::RIGHT), Direction::RIGHT) ? true : false);
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
    case 'G':
    case 'B':
    case 'Y':
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
Direction RandomGhostMove(Level& level, Monster& monster)
{
    Direction newDirection = Direction::NONE;
    Coord move = { move.row = monster.curr_pos.row, move.col = monster.curr_pos.col };
    int unsigned seed = (int)(std::chrono::system_clock::now().time_since_epoch().count());
    srand(seed);

    if (monster.run_first_move) {
        // first move when on the run is to reverse direction
        switch (monster.curr_direction)
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
        !monster.run_first_move ? newDirection = static_cast<Direction>(randomNumber) : newDirection;
        if (CanMove(level, monster, move, newDirection, monster.curr_direction))
            return newDirection;
    } while (true);
}
int MoveMonster(Game& game, Level& level, Player& player, Monster& monster)
{
    Direction bestMove = Direction::NONE; // will be the next move

    if (monster.skip_turn || game.game_over || CheckCollision(player, monster))
    {
        return 0; // exit without making the move
    }

    if (monster.wait > 0)
    {
        monster.wait--; // monster has respawned and we are counting dowm the tics to let it move
        return 0; // exit without making a move
    }

    if (monster.mode == Mode::RUN) // make random moves truning around on first move
    {
        bestMove = RandomGhostMove(level, monster);
        DoMonsterMove(level, player, monster, bestMove);
        return 0;
    }

    if (monster.mode != Mode::RUN)  // run recursive AI for chase, roam and spawn modes 
    {
        int score, bestScore = monster.mode == Mode::RUN ? -1000 : 1000;
        Coord nextMove;
        Coord move = { move.row = monster.curr_pos.row, move.col = monster.curr_pos.col };
        Direction new_direction = Direction::NONE;

        for (int i = 0; i <= 3; i++) // cycle through up,down,left,right to find the valid best next move
        {
            new_direction = static_cast<Direction>(i);
            if (CanMove(level, monster, move, new_direction, monster.curr_direction)) // check for next available square
            {
                // set new coords of ghost
                nextMove = NewMove(level, monster, move, new_direction);
                monster.curr_pos.row = nextMove.row; monster.curr_pos.col = nextMove.col;

                // calculate distance score based on new coords
                int score = GetBestMove(level, player, monster, nextMove, new_direction, 1, true); // get the minimax score for the move (recurssive)

                // revert ghost coords back
                monster.curr_pos.row = move.row; monster.curr_pos.col = move.col;

                // if the new move score gets the ghost closer to the player than the current bestscore, set this as the best move
                if (monster.mode == Mode::RUN)
                {
                    if (score > bestScore)
                    {
                        bestScore = score; // set new best score
                        bestMove = new_direction; // set new best move direction
                    }
                }
                else
                {
                    if (score < bestScore)
                    {
                        bestScore = score; // set new best score
                        bestMove = new_direction; // set new best move direction
                    }
                }

            }
        }
        DoMonsterMove(level, player, monster, bestMove);
        return 0;
    }

    return 0; // clean exit
}
void DoMonsterMove(Level& level, Player& player, Monster& monster, Direction bestMove)
{
    monster.prev_pos.row = monster.curr_pos.row;
    monster.prev_pos.col = monster.curr_pos.col;
    monster.square_content_prior = monster.square_content_now;
    switch (bestMove)
    {
    case Direction::UP: //up
        monster.square_content_now = level.map[monster.curr_pos.row - 1][monster.curr_pos.col];
        monster.curr_pos.row--;
        monster.curr_direction = Direction::UP;
        break;
    case Direction::RIGHT: // right
        monster.square_content_now = level.map[monster.curr_pos.row][monster.curr_pos.col + 1];
        monster.curr_pos.col++;
        monster.curr_direction = Direction::RIGHT;
        break;
    case Direction::DOWN: // down
        monster.square_content_now = level.map[monster.curr_pos.row + 1][monster.curr_pos.col];
        monster.curr_pos.row++;
        monster.curr_direction = Direction::DOWN;
        break;
    case Direction::LEFT: // left
        monster.square_content_now = level.map[monster.curr_pos.row][monster.curr_pos.col - 1];
        monster.curr_pos.col--;
        monster.curr_direction = Direction::LEFT;
        break;
    default:
        break;
    }
    // if the sqaure the monster is looking at next is the player
    monster.square_content_now == 'C' ? monster.square_content_now = ' ' : monster.square_content_now;

    // if the mosnter is over the player save a blank space to buffer
    CheckCollision(player, monster) ? monster.square_content_now = ' ' : monster.square_content_now;
}

// Player/Monster Events and Status
void SetSuperPlayer(Game& game, Level& level, Player& player, Monster& monster)
{
    if (monster.is_edible)
    {
        monster.skip_turn = !monster.skip_turn; // when monster is edible slow him down
        game.g_t_now = chrono::high_resolution_clock::now(); // check the time now
        game.time_span = duration_cast<duration<double>>(game.g_t_now - game.g_t_start); // calc time difference
        if (game.time_span.count() >= level.edible_ghost_duration) { // if is edible has expired reset
            monster.is_edible = false;
            monster.run_first_move = true;
            monster.mode = Mode::CHASE;
            monster.rand = 5;
        }

    }
    else
    {
        monster.skip_turn = false;
    }
}
void SetPlayerState(Game& game, Level& level, Player& player, Monster& monster)
{
    char levelObj = level.map[player.curr_pos.row][player.curr_pos.col];
    switch (levelObj)
    {
    case '.': // eat pellet
        level.eaten_pellets++;
        break;
    case 'o': // eat power up
        monster.curr_pos.col != monster.spawn_pos.col ? monster.is_edible = true : monster.is_edible = false; // monster only edible on power up if out of spwan area
        monster.mode = Mode::RUN;
        monster.rand = 30; // increase random for better change to catch em
        game.g_t_start = chrono::high_resolution_clock::now();
        level.eaten_pellets++;
        break;
    }
}
void SetGhostMode(Level& level, Player& player, Monster& monster)
{
    if (monster.mode == Mode::CHASE || monster.mode == Mode::SPAWN) {
        level.chase_duration = duration_cast<duration<double>>(level.time_start - chrono::high_resolution_clock::now());
        if (abs(level.chase_duration.count()) > level.chase_for && level.roam_count > 0) // will roam four times only
        {
            level.roam_count--;
            monster.mode = Mode::ROAM;
            level.time_start = chrono::high_resolution_clock::now();
        }
    }

    if (monster.mode == Mode::ROAM) {
        level.roam_duration = duration_cast<duration<double>>(level.time_start - chrono::high_resolution_clock::now());
        if (abs(level.roam_duration.count()) > level.roam_for)
        {
            monster.mode = Mode::CHASE;
            level.time_start = chrono::high_resolution_clock::now();
        }
    }
    if (monster.mode == Mode::RUN) {
        level.time_start = chrono::high_resolution_clock::now();
    }
}
void PlayerMonsterCollision(Game& game, Level& level, Player& player, Monster& monster)
{
    if (CheckCollision(player, monster))
    {
        DrawLevel(level, player, monster); // print the move immediately
        if (monster.is_edible) // ghost dies
        {
            // if the monster was on a pellet/powerup sqaure need to count it up
            if (monster.square_content_now == Level::chr_pellet || monster.square_content_now == Level::chr_powerup)
                level.eaten_pellets++;
            level.eaten_ghots++; // increment ghosts eaten
            game.gobble_pause = true; // set small pause after eating ghost
            SpawnMonster(monster); // reset ghost to spawn area whihc resets mode to spawn
        }
        else // player dies
        {
            // if in ghosts in roam mode and player dies add one count to roam mode so it haappens again
            monster.mode == Mode::ROAM ? level.roam_count++ : level.roam_count;

            // replace ghost with empty sqaure since the player was there an any pellet got eaten
            level.map[monster.curr_pos.row][monster.curr_pos.col] = ' ';

            // end game or respawn player if no lives left
            player.lives--;
            if (player.lives > 0)
            {
                game.player_beat_pause = true; // give player time to get ready
                SpawnPlayer(player);
                SpawnMonster(monster);
            }
            else
            {
                game.game_over = true;
            }
        }
    }
}
bool CheckCollision(Player& player, Monster& monster)
{
    return(player.curr_pos.col == monster.curr_pos.col && player.curr_pos.row == monster.curr_pos.row ? true : false);
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
string GhostMode(Monster& monster)
{
    switch (monster.mode)
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
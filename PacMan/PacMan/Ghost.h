#pragma once
#include "Character.h"
#include "EnumsAndStatics.h"

class Ghost: public Character
{
private:

    // gets set once based on ghost type
    Ghosts name;
    char ghost;
    Coord chase_modifier;
    int color = 71;
    int wait = 15; // if player dies or at start stagger exit

    // gets set once based based on current level
    Coord spawn_target; // just above spawn area
    Coord roam_target; // roams to top right hand side
    
    // changes as the game / level changes
    Mode mode = Mode::CHASE;
    char square_content_now = ' ';
    char square_content_prior = ' ';
    bool run_first_move = true;
    bool is_edible = false;
    bool color_on = false;
    bool skip_turn = false; // use this to slow down moster if edible

public:
    // constructors
    Ghost();
    Ghost(Ghosts ghost);

    // methods
    int DistanceToPlayer(Coord player_current_position);
    int DistanceToRoamTarget();
    int DistanceToSpawnTarget();
    bool PlayerCollision(Coord player_coord);
    void DecreaseWait();
    void MoveGhost(const Coord player_coord, const Direction direction, const char map_content);

    // Getters
    Coord GetChaseModifier();
    Coord GetRoamTarget();
    Ghosts Name();
    char GetPreviousSqaureContent();
    char GetContentCurrent();
    Mode GetMode();
    bool IsEdible();
    bool FlashBlue();
    int GetColor();
    int GetWait();
    bool SkipTurn();
    Coord GetSpawnTarget();
    char GhostChar();
    bool ReverseMove();

    // Setters
    void SetPreviousSqaureContent(char content);
    void SetContentCurrent(char content);
    void SetContentCurrent(bool same);
    void SetMode(Mode mode);
    void SetEdible(bool edible);
    void SetFlashBlue(bool flash);
    void SetColor(int color);
    void SetWait(int wait);
    void SetSkipTurn(bool skip_turn);
    void SpawnGhost(bool player_died);
    void SetSpawnTarget(Coord spawn_target);
    void SetSpawnTarget(int row, int col);
    void SetReverseMove(bool reverse);
};
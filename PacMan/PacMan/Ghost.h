#pragma once
#include "Character.h"
#include "Enums.h"

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
    int look_ahead = 1; // how far ahead the IA looks for player

public:
    // constructors
    Ghost();
    Ghost(Ghosts ghost);

    // destructors
    ~Ghost();

    // methods
    int DistanceToPlayer(Coord player_current_position);
    int DistanceToRoamTarget();
    int DistanceToSpawnTarget();
    bool PlayerCollision(Coord player_coord);

    //encapsulation
    char GetPreviousSqaureContent();
    void SetPreviousSqaureContent(char content);
    char GetContentCurrent();
    void SetContentCurrent(char content);
    void SetContentCurrent(bool same);
    Mode GetMode();
    void SetMode(Mode mode);
    bool IsEdible();
    void SetEdible(bool edible);
    bool FlashBlue();
    void SetFlashBlue(bool flash);
    int GetColor();
    void SetColor(int color);
    int GetWait();
    void SetWait(int wait);
    void DecreaseWait();
    void SetSkipTurn(bool skip_turn);
    bool SkipTurn();
    void SetLookAhead(int moves);
    int GetLookAhead();
    void SpawnGhost(bool player_died);
    void SetSpawnTarget(Coord spawn_target);
    void SetSpawnTarget(int row, int col);
    Coord GetSpawnTarget();
    char GhostChar();
    void SetReverseMove(bool reverse);
    bool ReverseMove();
    Coord GetChaseModifier();
    Coord GetRoamTarget();
    Ghosts Name();
    
};
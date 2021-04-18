#pragma once
#include "Character.h"

class Ghost: public Character
{
    //class Game* p_game = nullptr; // fwd declare class

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
    bool is_edible = false;
    bool color_on = false;
    bool skip_turn = false; // use this to slow down moster if edible
    

public:
   
    // constructors
    Ghost();
    Ghost(Ghosts ghost);
    ~Ghost();

    // methods
    bool PlayerCollision();
    void DecreaseWait();
    void MoveGhost(const Direction direction);
    int MakeGhostMove();
    int GetBestMove(Coord current_position, Direction current_direction, int depth);
    void CoutGhost();

    // Getters
    Coord GetChaseModifier();
    Coord GetRoamTarget();
    Ghosts Name();
    Mode GetMode();
    bool IsEdible();
    bool FlashBlue();
    int GetColor();
    int GetWait();
    bool SkipTurn();
    Coord GetSpawnTarget();
    char GhostChar();
    bool GameRefIsSet();

    // Setters
    void SetMode(Mode mode);
    void SetEdible(bool edible);
    void SetFlashBlue(bool flash);
    void SetColor(int color);
    void SetWait(int wait);
    void SetRoamTarget(Coord roam_target);
    void SetSkipTurn(bool skip_turn);
    void SpawnGhost(Ghosts name, bool player_died);
    void SetSpawnTarget(Coord spawn_target);
    void SetSpawnTarget(int row, int col);
    void SetReverseMove(bool reverse);

    //setter for forward class
    void SetGameRef(Game* p_game);
};
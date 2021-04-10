#include "Game.h"

Player::Player() 
{

};


// methods
void Player::AddLives(int number)
{
	lives += number;
}

void Player::TakeLives(int number)
{
	lives -= number;
}

void Player::ReSpawn()
{
	//previous_position = current_position;
	current_position = spawn_position;
	bool is_super = false;
	current_direction = Direction::LEFT;
	previous_direction = Direction::LEFT;
}

void Player::EatGhost(int ghost_index)
{
	ghosts_eaten[ghost_index] = true;
}

void Player::EatGhost(char character)
{
	switch (character)
	{
	case 'R':
		ghosts_eaten[0];
		break;
	case 'Y':
		ghosts_eaten[1];
		break;
	case 'B':
		ghosts_eaten[2];
		break;
	case 'P':
		ghosts_eaten[3];
		break;

	}
}

void Player::ClearEatenGohsts()
{
	for (int i = 0; i < sizeof(ghosts_eaten); i++)
	{
		ghosts_eaten[i] = false;
	}
}

bool Player::AllGhostsEaten()
{
	for (int i = 0; i < sizeof(ghosts_eaten); i++)
	{
		if (ghosts_eaten[i] == false)
			return false;
	}
	return true;
}

bool Player::HasNoLives()
{
	return(lives < 1 ? true : false);
}

void Player::MovePlayer()
{
	previous_position = current_position;
	Coord next_position(current_position, previous_direction);

	switch (current_direction)
	{
	case Direction::UP: //up
		if (p_game->p_level->NotWall({ current_position.row - 1, current_position.col }, Direction::UP)) {
			next_position.SetTo(current_position, Direction::UP);
			previous_direction = Direction::UP;
		}
		break;
	case Direction::RIGHT: // right
		if (p_game->p_level->NotWall({ current_position.row, current_position.col + 1 }, Direction::RIGHT)) {
			next_position.SetTo(current_position, Direction::RIGHT);
			previous_direction = Direction::RIGHT;
		}
		break;
	case Direction::DOWN: // down
		if (p_game->p_level->NotWall({ current_position.row + 1, current_position.col }, Direction::DOWN)) {
			next_position.SetTo(current_position, Direction::DOWN);
			previous_direction = Direction::DOWN;
		}
		break;
	case Direction::LEFT: // left
		if (p_game->p_level->NotWall({ current_position.row, current_position.col - 1}, Direction::LEFT)) {
			next_position.SetTo(current_position, Direction::LEFT);
			previous_direction = Direction::LEFT;
		}
		break;
	}

	// if the player was unable to do the move then keep going until it hits a wall
	switch (previous_direction)
	{
	case Direction::UP:
		if (!p_game->p_level->NotWall({ current_position.row - 1, current_position.col }, Direction::UP))
			next_position = current_position;
		break;
	case Direction::RIGHT:
		if (!p_game->p_level->NotWall({ current_position.row, current_position.col + 1}, Direction::RIGHT))
			next_position = current_position;
		break;
	case Direction::DOWN:
		if (!p_game->p_level->NotWall({ current_position.row + 1, current_position.col }, Direction::DOWN))
			next_position = current_position;
		break;
	case Direction::LEFT:
		if (!p_game->p_level->NotWall({ current_position.row, current_position.col - 1}, Direction::LEFT))
			next_position = current_position;
		break;
	}

	// move the player
	current_position = next_position;
}

bool Player::PayerGhostCollision(int ghost_index)
{
	return (current_position == p_game->p_ghosts[ghost_index]->GetCurrentPosition());
}

void Player::CoutPlayer()
{
	Draw::SetColor(Globals::cPLAYER);
	if (chomp) { // mouth closed
		
		switch (previous_direction)
		{
		case Direction::UP:
			cout << char(Globals::pacman_up_closed);
			break;
		case Direction::RIGHT:
			cout << char(Globals::pacman_right_closed);
			break;
		case Direction::DOWN:
			cout << char(Globals::pacman_down_closed);
			break;
		case Direction::LEFT:
			cout << char(Globals::pacman_left_closed);
			break;
		default:
			cout << char(Globals::pacman_left_closed);
			break;
		}
		chomp = !chomp;
	}
	else // mouth open
	{
		switch (previous_direction)
		{
		case Direction::UP:
			cout << char(Globals::pacman_up_open);
			break;
		case Direction::RIGHT:
			cout << char(Globals::pacman_right_open);
			break;
		case Direction::DOWN:
			cout << char(Globals::pacman_down_open);
			break;
		case Direction::LEFT:
			cout << char(Globals::pacman_left_open);
			break;
		default:
			cout << char(Globals::pacman_left_open);
			break;
		}
		player_move_content == ' ' ? chomp = false : chomp = !chomp;
	}
}


// getters
int Player::Lives()
{
	return(lives);
}

void Player::PowerUp(bool powerup)
{
	is_super = powerup;
}

int Player::GetScore()
{
	return(score);
}

char Player::GetMovedIntoSquareContents()
{
	return player_move_content;
}

Coord Player::GetCurrentPosition()
{
	return current_position;
}

bool Player::GameRefIsSet()
{
	return (p_game ? true : false);
}


// setters
void Player::SetLives()
{
	lives = lives;
}

void Player::SetLives(int number)
{
	lives = number;
}

void Player::SetScore(int score)
{
	this->score = score;
}

void Player::SetMovedIntoSquareContents(char ascii)
{
	player_move_content = ascii;
}

void Player::SetGameRef(Game* p_game)
{
	this->p_game = p_game;
}
#ifndef __enemy_H
#define __enemy_H

#include "entity.h"
#include <stdlib.h>
#include <vector>

enum AiState
{
	FOLLOW,
	ATTACK,
	HIDE,
	TURN
};
class shell;

class enemy : public entity {

public:
	enemy();
	~enemy();
	int enemyID;
	AiState state;
	entity* target;
	int toFire;
	MathVector2D<float> spawnPos;
	void init(entity* tankTurret, float x, float y, int id, int priority, SpriteColorFormat color, SpriteSize size, float thrust, float maxSpeed, int matrixID, int paletteID, int health);
	void draw(camera* c);
	void moveShip();
	entity* tankTurret;
	void accelerate(bool dir);
	void process(vector <shell*>* shellVector);
	bool willItCollideWith(entity& e);
	float getDistanceTo(entity* e);
	void rotateToTarget();
	void fireShell(vector <shell*>* shellVector);

};

#endif
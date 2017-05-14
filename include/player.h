#ifndef __player_H
#define __player_H

#include "entity.h"

class player : public entity {

public:
	player();
	~player();
	void init(entity* tankTurret, float x, float y, int id, int priority, SpriteColorFormat color, SpriteSize size, float thrust, float maxSpeed, int matrixID, int paletteID, int health);
	void draw(camera* c);
	void moveShip();
	entity* tankTurret;
	void accelerate(bool dir);
	bool willItCollideWith(entity& e);
};

#endif
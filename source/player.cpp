#include "player.h"
#include <stdlib.h>
#include <stdio.h>


player::player(){
	
}

void
player::init(entity* tankTurret, float x, float y, int id, int priority, SpriteColorFormat color, SpriteSize size, float thrust, float maxSpeed, int matrixID, int paletteID, int health){
    this->tankTurret = tankTurret;
    entity::init(x, y, id, priority, color, size, thrust, maxSpeed, matrixID, paletteID, health);
}

void
player::draw(camera* c){
	//printf("(%f, %f) \n", c->position.x, c->position.y);
	//printf("(%f, %f) \n", position.x, position.y);
	//printf("(%f, %f) \n", (position.x - c->position.x), (position.y - c->position.y));


	if(health <= 0){
		tankTurret->matrixID = -1;
		matrixID = - 1;
		oamSetAffineIndex(&oamMain, spriteId, -1, false);
		oamSetAffineIndex(&oamMain, tankTurret->spriteId, -1, false);
	}
	if(c->position.x - 70 > position.x || c->position.y - 70 > position.y || + c->size.x < position.x -c->position.x || c->size.y < position.y - c->position.y){

	}
	else {
		oamSet(&oamMain, //main graphics engine context
	            tankTurret->spriteId,           //oam index (0 to 127)  
	            tankTurret->position.x - c->position.x,
	            tankTurret->position.y - c->position.y ,   //x and y pixle location of the sprite
	            0,                    //priority, lower renders last (on top)
	            tankTurret->paletteID,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
	            tankTurret->size,     
	            tankTurret->color, 
	            tankTurret->gfxIndex,                  //pointer to the loaded graphics
	            tankTurret->matrixID,                  //sprite rotation data  
	            false,               //double the size when rotating?
	            false,          //hide the sprite?
	            false, false, //vflip, hflip
	            false   //apply mosaic
	            ); 

	     oamSet(&oamMain, //main graphics engine context
	            spriteId,           //oam index (0 to 127)  
	            position.x - c->position.x,
	            position.y - c->position.y ,   //x and y pixle location of the sprite
	            1,                    //priority, lower renders last (on top)
	            paletteID,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
	            size,     
	            color, 
	            gfxIndex,                  //pointer to the loaded graphics
	            matrixID,                  //sprite rotation data  
	            false,               //double the size when rotating?
	            false,          //hide the sprite?
	            false, false, //vflip, hflip
	            false   //apply mosaic
	            );
 	}
     if(health <= 0){
     	oamSetHidden(&oamMain, spriteId, true);
     	oamSetHidden(&oamMain, tankTurret->spriteId, true);
     }
}

player::~player() {
	delete(tankTurret);
}


void player::moveShip() {
	//move the ship
	if(position.x + velocity.x > 0 && position.x + velocity.x < 960){
		position.x += velocity.x;
	}
	if(position.y + velocity.y > 0  && position.y + velocity.y < 704){
		position.y += velocity.y;
	}

	
	if(matrixID != -1){
		rotateSprite(matrixID, getAngleDeg());
	}

	//move turret
	if(tankTurret->position.x + tankTurret->velocity.x> 0 && tankTurret->position.x + tankTurret->velocity.x < 960) {
		tankTurret->position.x += tankTurret->velocity.x;
	}
	if(tankTurret->position.y + tankTurret->velocity.y > 0 && tankTurret->position.y + tankTurret->velocity.y < 704){
		tankTurret->position.y += tankTurret->velocity.y;
	}
	if(tankTurret->matrixID != -1){
		tankTurret->rotateSprite(tankTurret->matrixID, tankTurret->getAngleDeg());
	}
}

void player::accelerate(bool dir) {
	
	
	float incX;
	float incY;
	
	if(dir == false){
		incX = thrust * sin(angle);
		incY = thrust * cos(angle);

	}
	else {
		incX = thrust * sin(angle-PI);
		incY = thrust * cos(angle-PI);
	}
	
	
    //the following method of speed limitation is not accurate, traveling
    //diagonally is faster than straight, which is not the desired limitation
	//a more accurate method is needed at a later time
	
	velocity.x += incX;
	tankTurret->velocity.x += incX;
	//make sure can't go too fast in x direction
	if (velocity.x > maxSpeed) {
		velocity.x = maxSpeed;
		tankTurret->velocity.x = maxSpeed;
	}
	if (velocity.x < -maxSpeed) {
		velocity.x = -maxSpeed;
		tankTurret->velocity.x = -maxSpeed;
	}
	
	velocity.y += incY;
	tankTurret->velocity.y += incY;
	//make sure can't go too fast in y direction
	if (velocity.y > maxSpeed) {
		velocity.y = maxSpeed;
		tankTurret->velocity.y = maxSpeed;
	}
	if (velocity.y < -maxSpeed) {
		velocity.y = -maxSpeed;
		tankTurret->velocity.y = -maxSpeed;
	}



}


bool
player::willItCollideWith(entity& e)
{
	MathVector2D<int> enity = e.spriteSizetoInt();
	MathVector2D<int> current = spriteSizetoInt();

	// Ex006.4: Generic Entity Collision routine.
	int width1 = (current.x / 2);
	int width2 = (enity.x / 2);

	int height1 = (current.y / 2);
	int height2 = (enity.y/ 2);

	

	float x1 = position.x + velocity.x + (width1 / 2);
	float y1 = position.y + velocity.y + (height1 / 2);

	float x2 = e.position.x + (width2 / 2);
	float y2 = e.position.y + (height2 / 2);


	// Ex006.4: Does this object collide with the e object?
	// Ex006.4: Create a circle for each entity (this and e).

	// Ex006.4: Check for intersection.
	// Ex006.4: Using circle-vs-circle collision detection.
	if ((sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)) - (width2)-(width1)) <= 0){
		return true;
	}
	else {
		return false;
	}
	// Ex006.4: Return result of collision.

}
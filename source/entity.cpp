#include "entity.h"
#include <stdlib.h>
#include <stdio.h>

entity::entity():
 priority(0)
, spriteId(0)
{

	this->position.x = -10;
	this->position.y = -10;
	this->matrixID = -1;

}

entity::~entity(){

}

void
entity::init(float x, float y, int id, int priority, SpriteColorFormat color, SpriteSize size, float thrust, float maxSpeed, int matrixIDOG, int paletteID, int health){
	this->position.x = x;
	this->position.y = y;
	this->priority = priority;
	spriteId = id;
	this->paletteID = paletteID;
	this->color = color;
	this->size = size;
	this->velocity.x = 0;
	this->velocity.y = 0;
	this->health = health;
	angle = 0.0;
	this->thrust = thrust;
	this->maxSpeed = maxSpeed;
	this->matrixID = matrixIDOG;
}

void
entity::draw(camera* c){
	bool todraw = (this->health <= 0);

	if(c->position.x - 70 > position.x || c->position.y - 70 > position.y || + c->size.x < position.x -c->position.x || c->size.y < position.y - c->position.y){

	}
	else {
		oamSet(&oamMain, //main graphics engine context
            spriteId,           //oam index (0 to 127)  
            position.x - c->position.x,
            position.y - c->position.y ,   //x and y pixle location of the sprite
            priority,                    //priority, lower renders last (on top)
            paletteID,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            size,     
            color, 
            gfxIndex,                  //pointer to the loaded graphics
            matrixID,                  //sprite rotation data  
            false,               //double the size when rotating?
            todraw,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        );   
	}

}


int entity::radToDeg(float rad) {
	return (int)(rad * (DEGREES_IN_CIRCLE/(2 * PI)));
}

int entity::getAngleDeg() {
	return radToDeg(angle);
}

float entity::getAngleRad() {
	return angle;
}

void entity::accelerate(bool dir) {
	
	
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
	//make sure can't go too fast in x direction
	if (velocity.x > maxSpeed) {
		velocity.x = maxSpeed;
	}
	if (velocity.x < -maxSpeed) {
		velocity.x = -maxSpeed;
	}
	
	velocity.y += incY;
	//make sure can't go too fast in y direction
	if (velocity.y > maxSpeed) {
		velocity.y = maxSpeed;
	}
	if (velocity.y < -maxSpeed) {
		velocity.y = -maxSpeed;
	}
}

void entity::stopShip(){
	velocity.x = 0.0;
	velocity.y = 0.0;
}

void entity::moveShip() {
	//move the ship
	position.x += velocity.x;
	position.y += velocity.y;

	if(matrixID != -1) {
		rotateSprite(matrixID, getAngleDeg());
	}
}

void entity::rotateSprite(int id, int angle) {
    s16 s = sinLerp(angle) >> 4;
    s16 c = cosLerp(angle) >> 4;
    if(id >= 0 || id <= 31) {
	   oamAffineTransformation(&oamMain, id, c, s, -s, c);
	}
}

void entity::turnClockwise() {
	angle -= .0314159265358979323846264338327;
}

void entity::turnCounterClockwise() {
	angle += .0314159265358979323846264338327;
}

void entity::setAngle(float f){
	angle = f;
}


bool
entity::IsCollidingWith(entity& e)
{
	MathVector2D<int> enity = e.spriteSizetoInt();
	MathVector2D<int> current = spriteSizetoInt();

	// Ex006.4: Generic Entity Collision routine.
	int width1 = (current.x / 2);
	int width2 = (enity.x / 2);

	int height1 = (current.y / 2);
	int height2 = (enity.y/ 2);

	

	float x1 = position.x + (width1 / 2);
	float y1 = position.y + (height1 / 2);

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

MathVector2D<int> entity::spriteSizetoInt(){
	MathVector2D<int> returnvalue;
	if(size == SpriteSize_8x8){
		returnvalue.x = 8;
		returnvalue.y = 8;
	}
	else if(size == SpriteSize_16x16){
		returnvalue.x = 16;
		returnvalue.y = 16;
	}
	else if(size == SpriteSize_32x32){
		returnvalue.x = 32;
		returnvalue.y = 32;
	}
	else if(size == SpriteSize_64x64){
		returnvalue.x = 64;
		returnvalue.y = 64;
	}
	else if(size == SpriteSize_16x8){
		returnvalue.x = 16;
		returnvalue.y = 8;
	}
	else if(size == SpriteSize_32x8){
		returnvalue.x = 32;
		returnvalue.y = 8;
	}
	else if(size == SpriteSize_32x16){
		returnvalue.x = 32;
		returnvalue.y = 16;
	}
	else if(size == SpriteSize_64x32){
		returnvalue.x = 64;
		returnvalue.y = 32;
	}
	else if(size == SpriteSize_8x16){
		returnvalue.x = 8;
		returnvalue.y = 16;
	}
	else if(size == SpriteSize_8x32){
		returnvalue.x = 8;
		returnvalue.y = 32;
	}
	else if(size == SpriteSize_16x32){
		returnvalue.x = 16;
		returnvalue.y = 32;
	}
	else if(size == SpriteSize_32x64){
		returnvalue.x = 32;
		returnvalue.y = 64;
	}
	return returnvalue;
}
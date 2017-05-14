#ifndef __Entity_H__
#define __Entity_H__

#include <nds.h>
#include <math.h>
#include "camera.h"


#define PI (3.14159265358979323846264338327)


using namespace std;



class entity{
	public:

		entity();
		~entity();

		int priority;
		int spriteId;
		MathVector2D<float> position;
		int health;
		MathVector2D<float> velocity;
    	float angle; //in radians
    	int matrixID;
		float thrust;
    	float maxSpeed;
    	int paletteID;

		u16* gfxIndex;
		SpriteSize size;
		SpriteColorFormat color;


		void init(float x, float y, int id, int priority, SpriteColorFormat color, SpriteSize size, 
			float thrust, float maxSpeed, int matrixID, int paletteID, int health);
		void draw(camera* c);

		void rotateSprite(int id, int angle);
		int radToDeg(float rad);
    	int getAngleDeg();
  		float getAngleRad();
		void accelerate(bool dir);
		void stopShip();
		void moveShip();
    	void turnClockwise();
    	void turnCounterClockwise();
    	void setAngle(float f);

    	MathVector2D<int> spriteSizetoInt();
		bool IsCollidingWith(entity& e);

};

#endif
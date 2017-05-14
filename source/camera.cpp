#include "camera.h"


camera::camera(){
	position.x = 0;
	position.y = 0;
	
	size.x = 256;
	size.y = 192;
}

camera::~camera(){
	
}

void camera::move(float x, float y){
	position.x = x;
	position.y = y;
}

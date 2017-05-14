#include "shell.h"



shell::shell(){
	
}


shell::~shell(){
	
}

void
shell::draw(camera* c, u16* gfx, int oam){
	bool todraw = (this->health <= 0);

	if(c->position.x - 50 > position.x || c->position.y - 60 > position.y || c->position.x + c->size.x + 10 < position.x || c->position.y + c->size.y + 10 < position.y){

	}
	else {
		oamSet(&oamMain, //main graphics engine context
            oam,           //oam index (0 to 127)  
            position.x - c->position.x,
            position.y - c->position.y ,   //x and y pixle location of the sprite
            priority,                    //priority, lower renders last (on top)
            paletteID,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            size,     
            color, 
            gfx,                  //pointer to the loaded graphics
            matrixID,                  //sprite rotation data  
            false,               //double the size when rotating?
            todraw,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        );   
	}

}

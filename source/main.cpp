//World size 1024x768 = *4 the ds screen resolution 256*192

#include <nds.h>
#include <maxmod9.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <time.h>

#include "entity.h"
#include "player.h"
#include "shell.h"
#include "camera.h"
#include "enemy.h"

/* Backgrounds */
#include "drunkenlogo.h"
#include "victory.h"
#include "subbackground.h"
#include "gameover.h"
#include "start.h"
#include "substart.h"
/* Sprites */
#include "tank.h"
#include "shellBullet.h"
#include "subspawn.h"
#include "turret.h"
#include "mapTank.h"
#include "redmapTank.h"
#include "yellowmapTank.h"
#include "bluemapTank.h"
#include "redtank.h"
#include "spawn.h"
#include "yellowtank.h"
#include "bluetank.h"
/* Sounds */
#include "soundbank.h"
#include "soundbank_bin.h"

player* man;
enemy* enemy1;
enemy* enemy2;
enemy* enemy3;

touchPosition touch;
vector <shell*> shellVector;
std::vector<u16*> shellGFX;
entity* tankTurret;
entity* enemyTurret;
entity* enemy2Turret;
entity* enemy3Turret;

entity* spawn1;
entity* spawn2;
entity* spawn3;
entity* spawn4;

u16* mapP;
u16* map1;
u16* map2;
u16* map3;
u16* ssp1;
u16* ssp2;
u16* ssp3;
u16* ssp4;

int bgSub;
int bg3;

bool debug;
bool paused;
bool gameover;
bool restart;

MathVector2D<int> manSub;
MathVector2D<int> eSub;
MathVector2D<int> e2Sub;
MathVector2D<int> e3Sub;

mm_sfxhand thrU = 0;
mm_sfxhand thrD = 0;
mm_sfxhand thrL = 0;
mm_sfxhand thrR = 0;
mm_sfxhand sht = 0;
mm_sound_effect thrust  = {
        { SFX_MOVING } ,         // id
        (int)(1.0f * (1<<10)),  // rate
        0,      // handle
        255,    // volume
        127,      // panning
    };
mm_sound_effect explode  = {
        { SFX_EXPLODE } ,         // id
        (int)(1.0f * (1<<10)),  // rate
        0,      // handle
        255,    // volume
        127,      // panning
    };
mm_sound_effect shooting  = {
        { SFX_SHOOTING } ,         // id
        (int)(1.0f * (1<<10)),  // rate
        0,      // handle
        255,    // volume
        127,      // panning
    };
camera* c;


/*Old way of spawning shell
void spawnShell(entity * e){
    if(shellVector.size() < 100){
        entity *s = new entity;
        s->init(e->position.x+28, e->position.y+32, shellVector.size()+10, 2, SpriteColorFormat_16Color, SpriteSize_8x8, 3.0, 10.0, -1, 1, 5);
        s->matrixID = -1;
        s->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_16Color);
        dmaCopy(shellTiles, s->gfxIndex, 32*32);
        oamSetAffineIndex(&oamMain, s->spriteId, -1, false);
        dmaCopy(shellPal, &SPRITE_PALETTE[16], 512);
        s->angle = e->angle;
        s->accelerate(true);
        shellVector.push_back(s);
    } 
}
*/

void spawnShell(entity * e){
    shell *s = new shell;
    s->init(e->position.x+28, e->position.y+32, -1, 2, SpriteColorFormat_16Color, SpriteSize_8x8, 5.0, 10.0, -1, 4, 5);
    s->angle = e->angle;
    s->accelerate(true);
    s->firedByMan = 0;
    shellVector.push_back(s);
}

void initOAMShells(){
    //shellVector.size()+10

    for(int i = 0; i < 100; i++) {
        shellGFX.insert(shellGFX.begin()+i, oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_16Color));
        dmaCopy(shellBulletTiles, shellGFX[i], 32*32);
    }
        dmaCopy(shellBulletPal, &SPRITE_PALETTE[64], 512);
}

void handleInput() {
    scanKeys();
    touchRead(&touch);
    if(!paused){

        if (keysDown() & KEY_UP) {       thrU = mmEffectEx(&thrust);    }
        if (keysDown() & KEY_LEFT) {        thrL = mmEffectEx(&thrust);   }
        if (keysDown() & KEY_RIGHT) {        thrR = mmEffectEx(&thrust);   }
        if (keysDown() & KEY_DOWN) {        thrD = mmEffectEx(&thrust);    }
        
        if (keysHeld() & KEY_UP) {        man->accelerate(true);    } 
        else if (keysHeld() & KEY_DOWN) {        man->accelerate(false);    }
        
        if (keysHeld() & KEY_LEFT) {        man->turnCounterClockwise();    }
         else if (keysHeld() & KEY_RIGHT) {       man->turnClockwise();    }
        
        if(keysDown() & KEY_L){        spawnShell(man->tankTurret);   sht = mmEffectEx(&shooting);  }
        if(keysDown() & KEY_A){        man->setAngle(man->angle - (PI/2));    }
        if(keysDown() & KEY_B){        man->setAngle(man->angle + (PI/2));    }

        if(keysUp() & KEY_A){
            mmEffectCancel(sht);
        }

        if(keysDown() & KEY_X){
            enemy1->health = 0;
            enemy2->health = 0;
            enemy3->health = 0;
        }

        
        
        if (keysUp() & KEY_UP) {
            man->stopShip();
            man->tankTurret->stopShip();
            mmEffectCancel(thrU);
        }
        if (keysUp() & KEY_DOWN) {
                man->stopShip();
                man->tankTurret->stopShip();
                mmEffectCancel(thrD);
        }

        if (keysUp() & KEY_LEFT) {        mmEffectCancel(thrL);   }
        if (keysUp() & KEY_RIGHT) {        mmEffectCancel(thrR);   }



        
        
        if(keysHeld() & KEY_TOUCH){
            MathVector2D<int> defaultPosition;
            defaultPosition.x = SCREEN_WIDTH / 2;
            defaultPosition.y = SCREEN_HEIGHT / 2;

            float angle = atan2(touch.py - defaultPosition.y, touch.px - defaultPosition.x );
            man->tankTurret->setAngle(-(angle+(PI/2)));
        }
    }
    else {
        if(keysDown() & KEY_A){
            man->health = 0;
            paused = !paused;
        }
    }
    if(keysDown() & KEY_START){
            paused = !paused;
        }
}


void initSub(){
    mapP = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
    map1 = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
    map2 = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
    map3 = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
    dmaCopy(mapTankTiles, mapP, 16*16);
    dmaCopy(redmapTankTiles, map1, 16*16);
    dmaCopy(yellowmapTankTiles, map2, 16*16);
    dmaCopy(bluemapTankTiles, map3, 16*16);


    dmaCopy(mapTankPal, &SPRITE_PALETTE_SUB[0], 32);
    dmaCopy(redmapTankPal, &SPRITE_PALETTE_SUB[16], 32);
    dmaCopy(yellowmapTankPal, &SPRITE_PALETTE_SUB[32], 32);
    dmaCopy(bluemapTankPal, &SPRITE_PALETTE_SUB[48], 32);

    ssp1 = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
    ssp2 = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
    ssp3 = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
    ssp4 = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
    dmaCopy(subspawnTiles, ssp1, 16*16);
    dmaCopy(subspawnTiles, ssp2, 16*16);
    dmaCopy(subspawnTiles, ssp3, 16*16);
    dmaCopy(subspawnTiles, ssp4, 16*16);


}

void drawSubScreen(){
    bool todrawP = (man->health <= 0);
    bool todraw1 = (enemy1->health <= 0);
    bool todraw2 = (enemy2->health <= 0);
    bool todraw3 = (enemy3->health <= 0);

    manSub.x = man->position.x / 4;
    manSub.y = man->position.y / 4;
    eSub.x = enemy1->position.x / 4;
    eSub.y = enemy1->position.y / 4;
    e2Sub.x = enemy2->position.x / 4;
    e2Sub.y = enemy2->position.y / 4;
    e3Sub.x = enemy3->position.x / 4;
    e3Sub.y = enemy3->position.y / 4;

    /*
    s16 ms = sinLerp(man->angle) >> 4;
    s16 mc = cosLerp(man->angle) >> 4;
    if(id >= 0 || id <= 31) {
       oamAffineTransformation(&oamSub, id, mc, ms, -ms, mc);
    }

    s16 s1 = sinLerp(enemy1->tankTurret->angle) >> 4;
    s16 c1 = cosLerp(enemy1->tankTurret->angle) >> 4;
    oamAffineTransformation(&oamSub, 1, c1, s1, -s1, c1);
    
    s16 s2 = sinLerp(enemy2->tankTurret->angle) >> 4;
    s16 c2 = cosLerp(enemy2->tankTurret->angle) >> 4;
    oamAffineTransformation(&oamSub, 2, c2, s2, -s2, c2);
    
    s16 s3 = sinLerp(enemy3->tankTurret->angle) >> 4;
    s16 c3 = cosLerp(enemy3->tankTurret->angle) >> 4;
    oamAffineTransformation(&oamSub, 3, c3, s3, -s3, c3);
    
    */

    oamSet(&oamSub, //main graphics engine context
            0,           //oam index (0 to 127) 

            manSub.x, manSub.y,   //x and y pixle location of the sprite
            0,                    //priority, lower renders last (on top)
            0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            SpriteSize_16x16,     
            SpriteColorFormat_16Color, 
            mapP,                  //pointer to the loaded graphics
            -1,                  //sprite rotation data  
            false,               //double the size when rotating?
            todrawP,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        );
    oamSet(&oamSub, //main graphics engine context
            1,           //oam index (0 to 127)  
            eSub.x, eSub.y,   //x and y pixle location of the sprite
            0,                    //priority, lower renders last (on top)
            1,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            SpriteSize_16x16,     
            SpriteColorFormat_16Color, 
            map1,                  //pointer to the loaded graphics
            -1,                  //sprite rotation data  
            false,               //double the size when rotating?
            todraw1,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        );   
    oamSet(&oamSub, //main graphics engine context
            2,           //oam index (0 to 127)  
            e2Sub.x, e2Sub.y,   //x and y pixle location of the sprite
            0,                    //priority, lower renders last (on top)
            2,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            SpriteSize_16x16,     
            SpriteColorFormat_16Color, 
            map2,                  //pointer to the loaded graphics
            -1,                  //sprite rotation data  
            false,               //double the size when rotating?
            todraw2,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        ); 
    oamSet(&oamSub, //main graphics engine context
            3,           //oam index (0 to 127)  
            e3Sub.x, e3Sub.y,   //x and y pixle location of the sprite
            0,                    //priority, lower renders last (on top)
            3,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            SpriteSize_16x16,     
            SpriteColorFormat_16Color, 
            map3,                  //pointer to the loaded graphics
            -1,                  //sprite rotation data  
            false,               //double the size when rotating?
            todraw3,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        );  
    oamSet(&oamSub, //main graphics engine context
            4,           //oam index (0 to 127) 
            120, 176,   //x and y pixle location of the sprite
            0,                    //priority, lower renders last (on top)
            0,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            SpriteSize_16x16,     
            SpriteColorFormat_16Color, 
            ssp1,                  //pointer to the loaded graphics
            -1,                  //sprite rotation data  
            false,               //double the size when rotating?
            false,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        );
    oamSet(&oamSub, //main graphics engine context
            5,          //oam index (0 to 127) 
            0, 88,   //x and y pixle location of the sprite
            0,                    //priority, lower renders last (on top)
            1,                   //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            SpriteSize_16x16,     
            SpriteColorFormat_16Color, 
            ssp2,                  //pointer to the loaded graphics
            -1,                  //sprite rotation data  
            false,               //double the size when rotating?
            false,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        );;
    oamSet(&oamSub, //main graphics engine context
            6,           //oam index (0 to 127) 
            120, 0,   //x and y pixle location of the sprite
            0,                    //priority, lower renders last (on top)
            2,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            SpriteSize_16x16,     
            SpriteColorFormat_16Color, 
            ssp3,                  //pointer to the loaded graphics
            -1,                  //sprite rotation data  
            false,               //double the size when rotating?
            false,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        );

    oamSet(&oamSub, //main graphics engine context
            7,           //oam index (0 to 127) 
            240, 88,   //x and y pixle location of the sprite
            0,                    //priority, lower renders last (on top)
            3,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
            SpriteSize_16x16,     
            SpriteColorFormat_16Color, 
            ssp4,                  //pointer to the loaded graphics
            -1,                  //sprite rotation data  
            false,               //double the size when rotating?
            false,          //hide the sprite?
            false, false, //vflip, hflip
            false   //apply mosaic
        );
}

void assignTarget(int id){
    //get closest alive to hunter
    float distanceToM;
    float distanceTo1;
    float distanceTo2;
    float distanceTo3;
    float targetDistance;

    switch (id){
        case 1:
            //get distance to possible targets
            if(enemy2->health > 0){
                distanceTo2 = enemy1->getDistanceTo(enemy2);
            }
            else{
                distanceTo2 = 90000;
            }
            if(enemy3->health > 0){
                distanceTo3 = enemy1->getDistanceTo(enemy3);
            }
            else{
                distanceTo3 = 90000;
            }
            distanceToM = enemy1->getDistanceTo(man);

            //compare distances
            targetDistance = distanceToM;

            if(targetDistance > distanceTo2){
                targetDistance = distanceTo2;
            }
            if(targetDistance > distanceTo3){
                targetDistance = distanceTo3;
            }

            //convert target distance to a target
            if(targetDistance == distanceTo3){
                enemy1->target = enemy3;
            }
            else if(targetDistance == distanceTo2){
                enemy1->target = enemy2;
            }
            else {
                enemy1->target = man;
            }
            break;
        case 2:
            //get distance to possible targets
            if(enemy1->health > 0){
                distanceTo1 = enemy2->getDistanceTo(enemy1);
            }
            else{
                distanceTo1= 90000;
            }
            if(enemy3->health > 0){
                distanceTo3 = enemy2->getDistanceTo(enemy3);
            }
            else{
                distanceTo3 = 90000;
            }
            distanceToM = enemy2->getDistanceTo(man);

            //compare distances
            targetDistance = distanceTo3;

            if(targetDistance > distanceToM){
                targetDistance = distanceToM;
            }
            if(targetDistance > distanceTo1){
                targetDistance = distanceTo1;
            }

            //convert target distance to a target
            if(targetDistance == distanceTo3){
                enemy2->target = enemy3;
            }
            else if(targetDistance == distanceTo1){
                enemy2->target = enemy1;
            }
            else {
                enemy2->target = man;
            }
            break;

        case 3:
             //get distance to possible targets
            if(enemy1->health > 0){
                distanceTo1 = enemy3->getDistanceTo(enemy1);
            }
            else{
                distanceTo1= 90000;
            }
            if(enemy2->health > 0){
                distanceTo2 = enemy3->getDistanceTo(enemy2);
            }
            else{
                distanceTo2 = 90000;
            }
            distanceToM = enemy3->getDistanceTo(man);

            //compare distances
            targetDistance = distanceTo1;

            if(targetDistance > distanceToM){
                targetDistance = distanceToM;
            }
            if(targetDistance > distanceTo2){
                targetDistance = distanceTo2;
            }

            //convert target distance to a target
            if(targetDistance == distanceTo2){
                enemy3->target = enemy2;
            }
            else if(targetDistance == distanceTo1){
                enemy3->target = enemy1;
            }
            else {
                enemy3->target = man;
            }
            break;

        default :
            break;
    }
}

void randomlyTarget(){
    int e1Target = (rand() % 3) + 1;
    int e2Target = (rand() % 3) + 1;
    int e3Target = (rand() % 3) + 1;

    if(e1Target == 1){
        enemy1->target = man;
    } else if(e1Target == 2){
        enemy1->target = enemy2;
    } else if(e1Target == 3){
        enemy1->target = enemy3;
    }

    if(e2Target == 1){
        enemy2->target = man;
    } else if(e2Target == 2){
        enemy2->target = enemy1;
    } else if(e2Target == 3){
        enemy2->target = enemy3;
    }

    if(e3Target == 1){
        enemy3->target = man;
    } else if(e3Target == 2){
        enemy3->target = enemy1;        
    } else if(e3Target == 3){
        enemy3->target = enemy2;
    } 
}

void startgame(){
    man = 0;
    spawn1 = 0;
    spawn2 = 0;
    spawn3 = 0;
    spawn4 = 0;
    enemy1 = 0;
    enemy2 = 0;
    c = 0;
    enemy3 = 0;
    tankTurret = 0;
    enemyTurret = 0;
    enemy2Turret = 0;
    enemy3Turret = 0;


    bgSub = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
    dmaCopy(drunkenlogoBitmap, bgGetGfxPtr(bg3), 256*256);
    dmaCopy(drunkenlogoPal, BG_PALETTE, 512);

    // Create entity and allocates sprite in vram, then copies sprite into vram
    tankTurret = new entity();
    tankTurret->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    tankTurret->init(480 , 704,
                                      1, 0, SpriteColorFormat_16Color, SpriteSize_64x64, 0.7, 1.5, 1, 0, 1);
    dmaCopy(turretTiles, tankTurret->gfxIndex, 64*64);

    man = new player;
    man->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    man->init(tankTurret, 480 , 704,
                                       0, 1, SpriteColorFormat_16Color, SpriteSize_64x64, 0.7, 1.5, 0, 0, 200);
    dmaCopy(tankTiles, man->gfxIndex, 64*64);
    dmaCopy(tankPal, &SPRITE_PALETTE[0], 16);


    enemyTurret = new entity();
    enemyTurret->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    enemyTurret->init(0 ,352, 2, 0, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0,        2, 1, 1);
    dmaCopy(turretTiles, enemyTurret->gfxIndex, 64*64);

    enemy1 = new enemy();
    enemy1->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    enemy1->init(enemyTurret, 0 ,352, 3, 1, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0, 3, 1, 200);
    dmaCopy(redtankTiles, enemy1->gfxIndex, 64*64);
    enemy1->setAngle(PI/2);
    enemy1->tankTurret->setAngle(PI/2);
        dmaCopy(redtankPal, &SPRITE_PALETTE[16], 16);

    enemy2Turret = new entity();
    enemy2Turret->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    enemy2Turret->init(480 ,0, 4, 0, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0,         4, 2, 1);
    dmaCopy(turretTiles, enemy2Turret->gfxIndex, 64*64);

    enemy2 = new enemy();
    enemy2->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    enemy2->init(enemy2Turret, 480 ,0, 5, 1, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0, 5, 2, 200);
    dmaCopy(yellowtankTiles, enemy2->gfxIndex, 64*64);
    dmaCopy(yellowtankPal, &SPRITE_PALETTE[32], 16);
    enemy2->setAngle(PI);
    enemy2->tankTurret->setAngle(PI);

    enemy3Turret = new entity();
    enemy3Turret->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    enemy3Turret->init(960, 352, 6, 0, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0,         6, 3, 1);
    dmaCopy(turretTiles, enemy3Turret->gfxIndex, 64*64);

    enemy3 = new enemy();
    enemy3->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    enemy3->init(enemy3Turret, 960, 352, 7, 1, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0, 7, 3, 200);
    enemy3->setAngle(PI+(PI/2));
    enemy3->tankTurret->setAngle(PI+(PI/2));

    dmaCopy(bluetankTiles, enemy3->gfxIndex, 64*64);
    dmaCopy(bluetankPal, &SPRITE_PALETTE[48], 16);

    enemy1->enemyID = 1;
    enemy2->enemyID = 2;
    enemy3->enemyID = 3;

    /*
    assignTarget(1);
    assignTarget(2);
    assignTarget(3);
    */

    randomlyTarget();//should assign target randomly

    spawn1 = new entity();
    spawn1->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    spawn1->init(480, 704, 8, 2, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0, -1, 0, 1);
    dmaCopy(spawnTiles, spawn1->gfxIndex, 64*64);

    spawn2 = new entity();
    spawn2->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    spawn2->init(0, 352, 9, 2, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0, -1, 1, 1);
    dmaCopy(spawnTiles, spawn2->gfxIndex, 64*64);

    spawn3 = new entity();
    spawn3->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    spawn3->init(480, 0, 10, 2, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0, -1, 2, 1);
    dmaCopy(spawnTiles, spawn3->gfxIndex, 64*64);

    spawn4 = new entity();
    spawn4->gfxIndex = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
    spawn4->init(960, 352, 11, 2, SpriteColorFormat_16Color, SpriteSize_64x64, 0.5, 1.0, -1, 3, 1);
    dmaCopy(spawnTiles, spawn4->gfxIndex, 64*64);


    
    initOAMShells();
    initSub();

    /*  Set up sound data.*/

    mmLoad(MOD_VALKYRIE);
    mmSetModuleVolume(200);
    mmStart(MOD_VALKYRIE, MM_PLAY_LOOP);

    c = new camera(); //160
    c->move(man->position.x-96, man->position.y-128);


    bool victory = false;
    gameover = false;
    paused = false;
    /*
    int subspawnLoc1 = 120 + (176*256);
        int subspawnLoc101 = subspawnLoc1 + (1*256);
        int subspawnLoc102 = subspawnLoc1 + (2*256);
        int subspawnLoc103 = subspawnLoc1 + (3*256);
        int subspawnLoc104 = subspawnLoc1 + (4*256);
        int subspawnLoc105 = subspawnLoc1 + (5*256);
        int subspawnLoc106 = subspawnLoc1 + (6*256);
        int subspawnLoc107 = subspawnLoc1 + (7*256);
        int subspawnLoc108 = subspawnLoc1 + (8*256);
        int subspawnLoc109 = subspawnLoc1 + (9*256);
        int subspawnLoc110 = subspawnLoc1 + (10*256);
        int subspawnLoc111 = subspawnLoc1 + (11*256);
        int subspawnLoc112 = subspawnLoc1 + (12*256);
        int subspawnLoc113 = subspawnLoc1 + (13*256);
        int subspawnLoc114 = subspawnLoc1 + (14*256);
        int subspawnLoc115 = subspawnLoc1 + (15*256);


    int subspawnLoc2 = (88*256);
        int subspawnLoc201 = subspawnLoc2 + (1*256);
        int subspawnLoc202 = subspawnLoc2 + (2*256);
        int subspawnLoc203 = subspawnLoc2 + (3*256);
        int subspawnLoc204 = subspawnLoc2 + (4*256);
        int subspawnLoc205 = subspawnLoc2 + (5*256);
        int subspawnLoc206 = subspawnLoc2 + (6*256);
        int subspawnLoc207 = subspawnLoc2 + (7*256);
        int subspawnLoc208 = subspawnLoc2 + (8*256);
        int subspawnLoc209 = subspawnLoc2 + (9*256);
        int subspawnLoc210 = subspawnLoc2 + (10*256);
        int subspawnLoc211 = subspawnLoc2 + (11*256);
        int subspawnLoc212 = subspawnLoc2 + (12*256);
        int subspawnLoc213 = subspawnLoc2 + (13*256);
        int subspawnLoc214 = subspawnLoc2 + (14*256);
        int subspawnLoc215 = subspawnLoc2 + (15*256);

    int subspawnLoc3 = 120;
        int subspawnLoc301 = subspawnLoc3 + (1*256);
        int subspawnLoc302 = subspawnLoc3 + (2*256);
        int subspawnLoc303 = subspawnLoc3 + (3*256);
        int subspawnLoc304 = subspawnLoc3 + (4*256);
        int subspawnLoc305 = subspawnLoc3 + (5*256);
        int subspawnLoc306 = subspawnLoc3 + (6*256);
        int subspawnLoc307 = subspawnLoc3 + (7*256);
        int subspawnLoc308 = subspawnLoc3 + (8*256);
        int subspawnLoc309 = subspawnLoc3 + (9*256);
        int subspawnLoc310 = subspawnLoc3 + (10*256);
        int subspawnLoc311 = subspawnLoc3 + (11*256);
        int subspawnLoc312 = subspawnLoc3 + (12*256);
        int subspawnLoc313 = subspawnLoc3 + (13*256);
        int subspawnLoc314 = subspawnLoc3 + (14*256);
        int subspawnLoc315 = subspawnLoc3 + (15*256);

    int subspawnLoc4 = 240 + (88*256);
        int subspawnLoc401 = subspawnLoc4 + (1*256);
        int subspawnLoc402 = subspawnLoc4 + (2*256);
        int subspawnLoc403 = subspawnLoc4 + (3*256);
        int subspawnLoc404 = subspawnLoc4 + (4*256);
        int subspawnLoc405 = subspawnLoc4 + (5*256);
        int subspawnLoc406 = subspawnLoc4 + (6*256);
        int subspawnLoc407 = subspawnLoc4 + (7*256);
        int subspawnLoc408 = subspawnLoc4 + (8*256);
        int subspawnLoc409 = subspawnLoc4 + (9*256);
        int subspawnLoc410 = subspawnLoc4 + (10*256);
        int subspawnLoc411 = subspawnLoc4 + (11*256);
        int subspawnLoc412 = subspawnLoc4 + (12*256);
        int subspawnLoc413 = subspawnLoc4 + (13*256);
        int subspawnLoc414 = subspawnLoc4 + (14*256);
        int subspawnLoc415 = subspawnLoc4 + (15*256);
    */
    for (;;) {
            handleInput();
            if(!paused){
            if(!man->willItCollideWith((*enemy1)) || enemy1->health <= 0){
                if(!man->willItCollideWith((*enemy2)) || enemy2->health <= 0){
                    if(!man->willItCollideWith((*enemy3)) || enemy3->health <= 0){
                        man->moveShip();
                    }
                }
            }
            spawn1->draw(c);
            spawn2->draw(c);
            spawn3->draw(c);
            spawn4->draw(c);


            if(enemy1->target->health <= 0){
                assignTarget(1);
            }
            if(enemy2->target->health <= 0){
                assignTarget(2);
            }
            if(enemy3->target->health <= 0){
                assignTarget(3);
            }

            if(enemy1->health > 0) {
                if(!enemy1->willItCollideWith((*man)) || man->health <= 0){
                    if(!enemy1->willItCollideWith((*enemy2)) || enemy2->health <= 0){
                        if(!enemy1->willItCollideWith((*enemy3)) || enemy3->health <= 0){
                            enemy1->process(&shellVector);
                            enemy1->moveShip();
                        }
                        else {
                            enemy1->setAngle(enemy1->angle-(PI/2));
                            enemy1->process(&shellVector);
                            enemy1->moveShip();
                        }
                    }
                    else {
                        enemy1->setAngle(enemy1->angle-(PI/2));
                        enemy1->process(&shellVector);
                        enemy1->moveShip();
                    }
                }
                else {
                    enemy1->setAngle(enemy1->angle-(PI/2));
                    enemy1->process(&shellVector);
                    enemy1->moveShip();
                }
            }
            
            if(enemy2->health > 0) {
                if(!enemy2->willItCollideWith((*man)) || man->health <= 0){
                    if(!enemy2->willItCollideWith((*enemy1)) || enemy1->health <= 0){
                        if(!enemy2->willItCollideWith((*enemy3)) || enemy3->health <= 0){
                            enemy2->process(&shellVector);
                            enemy2->moveShip();
                        }
                        else {
                            enemy2->setAngle(enemy2->angle-(PI/2));;
                            enemy2->process(&shellVector);
                            enemy2->moveShip();
                        }
                    }
                    else {
                        enemy2->setAngle(enemy2->angle-(PI/2));
                        enemy2->process(&shellVector);
                        enemy2->moveShip();
                    }
                }
                else {
                    enemy2->setAngle(enemy2->angle-(PI/2));
                    enemy2->process(&shellVector);
                    enemy2->moveShip();
                }
            }

            if(enemy3->health > 0) {
                if(!enemy3->willItCollideWith((*man)) || man->health <= 0){
                    if(!enemy3->willItCollideWith((*enemy1)) || enemy1->health <= 0){
                        if(!enemy3->willItCollideWith((*enemy2)) || enemy2->health <= 0){
                            enemy3->process(&shellVector);
                            enemy3->moveShip();
                        }
                        else {
                            enemy3->setAngle(enemy3->angle-(PI/2));
                            enemy3->process(&shellVector);
                            enemy3->moveShip();
                        }
                    }
                    else {
                        enemy3->setAngle(enemy3->angle-(PI/2));
                        enemy3->process(&shellVector);
                        enemy3->moveShip();
                    }
                }
                else {
                    enemy3->setAngle(enemy3->angle-(PI/2));
                    enemy3->process(&shellVector);
                    enemy3->moveShip();
                }
            }

            /*
            if(man->position.x >= c->position.x + c->size.x - 60){
                c->move(c->position.x+8, c->position.y);
            }
            else if(man->position.x <= c->position.x + 20){
                c->move(c->position.x-8, c->position.y);
            }
            if(man->position.y >= c->position.y + c->size.y - 60){
                c->move(c->position.x, c->position.y+8);
            }
            else if(man->position.y <= c->position.y + 20){
                c->move(c->position.x, c->position.y-8);
            }
            */

            //c->move((man->position.x-128)+32, (man->position.y-96)+32);

            if((man->position.x-128)+32 < 0){
            }
            else {
                if((man->position.x-128)+32 < 768){
                    c->move((man->position.x-128)+32, c->position.y);
                }
            }
            if((man->position.y-96)+32 < 0){
            }
            else {
                if((man->position.y-96)+32 < 576){
                    c->move(c->position.x, (man->position.y-96)+32);
                }
            }

            man->draw(c);
            enemy1->draw(c);
            enemy2->draw(c);
            enemy3->draw(c);

            int i = 12;
            for (vector<shell*>::iterator ShellIT = shellVector.begin(); ShellIT != shellVector.end();) {
                (*ShellIT)->moveShip();
                
                if(i < 128){
                    (*ShellIT)->spriteId = i;
                    (*ShellIT)->draw(c, shellGFX[i], i);
                    i++;
                }
                if((*enemy1).health > 0 && (*ShellIT)->health > 0 && (*ShellIT)->firedByMan != 1){
                    if((*ShellIT)->IsCollidingWith((*enemy1))){
                        mmEffectEx(&explode);
                        enemy1->health -= 50;
                        (*ShellIT)->health = -1;
                    }
                }
                if((*enemy2).health > 0 && (*ShellIT)->health > 0 && (*ShellIT)->firedByMan != 2){
                    if((*ShellIT)->IsCollidingWith((*enemy2))){
                        mmEffectEx(&explode);
                        enemy2->health -= 50;
                        (*ShellIT)->health = -1;
                    }
                }
                if((*enemy3).health > 0 && (*ShellIT)->health > 0 && (*ShellIT)->firedByMan != 3){
                    if((*ShellIT)->IsCollidingWith((*enemy3))){
                        mmEffectEx(&explode);
                        enemy3->health -= 50;
                        (*ShellIT)->health = -1;
                    }
                }
                if((*man).health > 0 && (*ShellIT)->health > 0&& (*ShellIT)->firedByMan != 0){
                    if((*ShellIT)->IsCollidingWith((*man))){
                        mmEffectEx(&explode);
                        man->health -= 50;
                        (*ShellIT)->health = -1;
                    }
                }
                
                if((*ShellIT)->health > 0){
                    if((*ShellIT)->position.x < -1000 || (*ShellIT)->position.y > 2000 || (*ShellIT)->position.y < -1000 || (*ShellIT)->position.x > 2000){
                        (*ShellIT)->health = -1;                    
                    }
                }
                ++ShellIT;
                
            }

            for (vector<shell*>::iterator ShellIT2 = shellVector.begin(); ShellIT2 != shellVector.end();) {
                if ((*ShellIT2)->health < 0){
                    //oamSetXY(&oamMain, (*ShellIT2)->spriteId, -10, -10);
                    ShellIT2 = shellVector.erase(ShellIT2);
                }
                else{
                    ShellIT2++;
                }
            }

            if(spawn1->IsCollidingWith((*man))) {
                if(man->health < 200 && man->health > 0)
                    man->health++;
            }
            if(spawn2->IsCollidingWith((*enemy1))){
                if(enemy1->health < 200 && enemy1->health > 0)
                    enemy1->health++;
            }
            if(spawn3->IsCollidingWith((*enemy2))){
                if(enemy2->health < 200 && enemy2->health > 0)
                    enemy2->health++;
            }
            if(spawn4->IsCollidingWith((*enemy3))){
                if(enemy3->health < 200 && enemy3->health > 0)
                    enemy3->health++;
            }



            //printf("(%f, %f) \n", c->position.x, c->position.y);
            //iprintf("%d \n", (*enemy).health);

            if((*enemy1).health <= 0 && (*enemy2).health <= 0 && (*enemy3).health <= 0){
                //do victory
                victory = true;
                break;
            }
            if(man->health <= 0){
                //do gameover
                gameover = true;
                break;
            }



            u16* videoMemorySub = bgGetGfxPtr(bgSub);         
            /*
            for(int j = 0; j < (man->health / 10); j++){
                videoMemorySub[(manSub.y * 256) + manSub.x + j] = ARGB16(1, 31, 0, 0);
            }
            */
            int manHealth = (man->health/10);
            int enemyHealth = (enemy1->health/10);
            int enemy2Health = (enemy2->health/10);
            int enemy3Health = (enemy3->health/10);

            if(debug == false) {
                for(int x = 0; x < 256*192; x++) {
                    int manIF = (manSub.y * 256) + manSub.x;
                    int enemyIF = (eSub.y * 256) + eSub.x;
                    int enemy2IF = (e2Sub.y * 256) + e2Sub.x;
                    int enemy3IF = (e3Sub.y * 256) + e3Sub.x;




                    if(manIF < x && manIF + manHealth > x){
                        videoMemorySub[x] = ARGB16(1, 31, 0, 0);
                    }
                    else if(enemyIF < x && enemyIF + enemyHealth > x){
                        videoMemorySub[x] = ARGB16(1, 31, 0, 0);
                    }
                    else if(enemy2IF < x && enemy2IF + enemy2Health > x){
                        videoMemorySub[x] = ARGB16(1, 31, 0, 0);
                    }
                    else if(enemy3IF < x && enemy3IF + enemy3Health > x){
                        videoMemorySub[x] = ARGB16(1, 31, 0, 0);
                    }
                    /*
                    else if((subspawnLoc1 < x && subspawnLoc1 + 16 > x) ||
                        (subspawnLoc101 < x && subspawnLoc101  + 16 > x) ||
                        (subspawnLoc102 < x && subspawnLoc102  + 16 > x) ||
                        (subspawnLoc103 < x && subspawnLoc103  + 16 > x) ||
                        (subspawnLoc104 < x && subspawnLoc104  + 16 > x) ||
                        (subspawnLoc105 < x && subspawnLoc105  + 16 > x) ||
                        (subspawnLoc106 < x && subspawnLoc106  + 16 > x) ||
                        (subspawnLoc107 < x && subspawnLoc107  + 16 > x) ||
                        (subspawnLoc108 < x && subspawnLoc108  + 16 > x) ||
                        (subspawnLoc109 < x && subspawnLoc109  + 16 > x) ||
                        (subspawnLoc110 < x && subspawnLoc110 + 16 > x) ||
                        (subspawnLoc111 < x && subspawnLoc111 + 16 > x) ||
                        (subspawnLoc112 < x && subspawnLoc112 + 16 > x) ||
                        (subspawnLoc113 < x && subspawnLoc113 + 16 > x) ||
                        (subspawnLoc114 < x && subspawnLoc114 + 16 > x) ||
                        (subspawnLoc115 < x && subspawnLoc115 + 16 > x)
                        ){
                        videoMemorySub[x] = ARGB16(1, 6, 31, 0);
                    }
                    else if((subspawnLoc2 < x && subspawnLoc2 + 16 > x) ||
                        (subspawnLoc201 < x && subspawnLoc201 +  16 > x) ||
                        (subspawnLoc202 < x && subspawnLoc202 +  16 > x) ||
                        (subspawnLoc203 < x && subspawnLoc203 +  16 > x) ||
                        (subspawnLoc204 < x && subspawnLoc204 +  16 > x) ||
                        (subspawnLoc205 < x && subspawnLoc205 +  16 > x) ||
                        (subspawnLoc206 < x && subspawnLoc206 +  16 > x) ||
                        (subspawnLoc207 < x && subspawnLoc207 + 16 > x) ||
                        (subspawnLoc208 < x && subspawnLoc208 + 16 > x) ||
                        (subspawnLoc209 < x && subspawnLoc209 + 16 > x) ||
                        (subspawnLoc210 < x && subspawnLoc210 + 16 > x) ||
                        (subspawnLoc211 < x && subspawnLoc211 + 16 > x) ||
                        (subspawnLoc212 < x && subspawnLoc212 + 16 > x) ||
                        (subspawnLoc213 < x && subspawnLoc213 + 16 > x) ||
                        (subspawnLoc214 < x && subspawnLoc214 + 16 > x) ||
                        (subspawnLoc215 < x && subspawnLoc215 + 16 > x)
                        ){
                        videoMemorySub[x] = ARGB16(1, 20, 0, 0);
                    }
                    else if((subspawnLoc3 < x && subspawnLoc3 + 16 > x) ||
                        (subspawnLoc301 < x && subspawnLoc301 +16 > x) ||
                        (subspawnLoc302 < x && subspawnLoc302 +16 > x) ||
                        (subspawnLoc303 < x && subspawnLoc303 +16 > x) ||
                        (subspawnLoc304 < x && subspawnLoc304 +16 > x) ||
                        (subspawnLoc305 < x && subspawnLoc305 +16 > x) ||
                        (subspawnLoc306 < x && subspawnLoc306 +16 > x) ||
                        (subspawnLoc307 < x && subspawnLoc307 +16 > x) ||
                        (subspawnLoc308 < x && subspawnLoc308 +16 > x) ||
                        (subspawnLoc309 < x && subspawnLoc309 +16 > x) ||
                        (subspawnLoc310 < x && subspawnLoc310 + 16 > x) ||
                        (subspawnLoc311 < x && subspawnLoc311 + 16 > x) ||
                        (subspawnLoc312 < x && subspawnLoc312 + 16 > x) ||
                        (subspawnLoc313 < x && subspawnLoc313 + 16 > x) ||
                        (subspawnLoc314 < x && subspawnLoc314 + 16 > x) ||
                        (subspawnLoc315 < x && subspawnLoc315 + 16 > x)
                        ){
                        videoMemorySub[x] = ARGB16(1, 31, 31, 0);
                    }
                    else if((subspawnLoc4 < x && subspawnLoc4 + 16 > x) ||
                        (subspawnLoc401 < x && subspawnLoc401 + 16 > x) ||
                        (subspawnLoc402 < x && subspawnLoc402 + 16 > x) ||
                        (subspawnLoc403 < x && subspawnLoc403 + 16 > x) ||
                        (subspawnLoc404 < x && subspawnLoc404 + 16 > x) ||
                        (subspawnLoc405 < x && subspawnLoc405 + 16 > x) ||
                        (subspawnLoc406 < x && subspawnLoc406 + 16 > x) ||
                        (subspawnLoc407 < x && subspawnLoc407 + 16 > x) ||
                        (subspawnLoc408 < x && subspawnLoc408 + 16 > x) ||
                        (subspawnLoc409 < x && subspawnLoc409 + 16 > x) ||
                        (subspawnLoc410 < x && subspawnLoc410 + 16 > x) ||
                        (subspawnLoc411 < x && subspawnLoc411 + 16 > x) ||
                        (subspawnLoc412 < x && subspawnLoc412 + 16 > x) ||
                        (subspawnLoc413 < x && subspawnLoc413 + 16 > x) ||
                        (subspawnLoc414 < x && subspawnLoc414 + 16 > x) ||
                        (subspawnLoc415 < x && subspawnLoc415 + 16 > x)
                        ){
                        videoMemorySub[x] = ARGB16(1, 3, 0, 17);
                    }
                    */
                    else {
                       videoMemorySub[x] = ARGB16(1, 10, 20, 10);
                    }
                }
            }

            }
            drawSubScreen();


            swiWaitForVBlank();

            oamUpdate(&oamMain);
            oamUpdate(&oamSub);
            for(int j = 12; j < 128; j++){
                oamSet(&oamMain, //main graphics engine context
                    j,           //oam index (0 to 127)  
                    -10,
                    -10,   //x and y pixle location of the sprite
                    3,                    //priority, lower renders last (on top)
                    4,                    //this is the palette index if multiple palettes or the alpha value if bmp sprite 
                    SpriteSize_8x8, SpriteColorFormat_16Color,     
                    shellGFX[j],                  //pointer to the loaded graphics
                    -1,                  //sprite rotation data  
                    false,               //double the size when rotating?
                    true,          //hide the sprite?
                    false, false, //vflip, hflip
                    false   //apply mosaic
                );
            }
    }
    oamClear(&oamMain, 0, 0);
    oamClear(&oamSub, 0, 0);

    delete tankTurret;
    delete enemyTurret;
    delete enemy2Turret;
    delete enemy3Turret;
    delete man;
    delete spawn1;
    delete spawn2;
    delete spawn3;
    delete spawn4;
    delete enemy1;
    delete enemy2;
    delete enemy3;
        delete c;


    for (vector<shell*>::iterator ShellIT2 = shellVector.begin(); ShellIT2 != shellVector.end();) {
        delete (*ShellIT2);
        ShellIT2 = shellVector.erase(ShellIT2);
    }
    mmStop();

    if(victory){
        bg3 = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0,0);
        bgSetPriority(bg3, 3);
        dmaCopy(victoryBitmap, bgGetGfxPtr(bg3), 256*256);
        dmaCopy(victoryPal, BG_PALETTE, 256*2);
        for(;;){
            scanKeys();
            printf("victory \n");
            if(keysDown() & KEY_START){
                //restart = true; not working
                break;
            }
            if(keysDown() & KEY_A)
                break;

            swiWaitForVBlank();

            oamUpdate(&oamMain);
            oamUpdate(&oamSub);
        }
    }

    if(gameover){
        bg3 = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0,0);
        bgSetPriority(bg3, 3);
        dmaCopy(gameoverBitmap, bgGetGfxPtr(bg3), 256*256);
        dmaCopy(gameoverPal, BG_PALETTE, 256*2);
        for(;;){
            printf("gameover \n");
            
            swiWaitForVBlank();

            oamUpdate(&oamMain);
            oamUpdate(&oamSub);

            scanKeys();
            if(keysDown() & KEY_START){
                //restart = true; not working
                break;
            }
            if(keysDown() & KEY_A)
                break;
        }
    }

}

int main() {
    debug = false;  //disable sub screen and enable console
    restart = false;

    /*  Turn on the 2D graphics core. */
    powerOn(POWER_ALL_2D);
    lcdMainOnTop();

    srand(time(NULL));

    videoSetMode(MODE_3_2D | // Set the graphics mode to Mode 5
                 DISPLAY_BG3_ACTIVE | // Enable BG3 for display
                 DISPLAY_SPR_ACTIVE | // Enable sprites for display
                 DISPLAY_SPR_1D       // Enable 1D tiled sprites
                 );

    
    videoSetModeSub(MODE_3_2D | // Set the graphics mode to Mode 5
                   DISPLAY_SPR_ACTIVE | // Enable sprites for display
                 DISPLAY_SPR_1D       |
                   DISPLAY_BG3_ACTIVE); // Enable BG3 for display

    //Set up display modes and memory assignments
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_BG_0x06020000);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);
    vramSetBankE(VRAM_E_MAIN_SPRITE);
    vramSetBankF(VRAM_F_MAIN_SPRITE_0x06410000 );
    vramSetBankI(VRAM_I_SUB_SPRITE);

    oamInit(&oamMain, SpriteMapping_1D_64, false);
    oamInit(&oamSub, SpriteMapping_1D_64, false);

	
    //start the debug console
    if(debug) {
    	consoleDemoInit();
        consoleDebugInit(DebugDevice_CONSOLE);
    }

    //make background
    bg3 = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0,0);
    bgSetPriority(bg3, 3);
    


    bgSub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0,0);

    /* Initialize maxmod using the memory based soundbank set up. */
    mmInitDefaultMem((mm_addr)soundbank_bin);

    mmLoadEffect(SFX_MOVING);
    mmLoadEffect(SFX_EXPLODE);
    mmLoadEffect(SFX_SHOOTING);

    dmaCopy(startBitmap, bgGetGfxPtr(bg3), 256*256);
    dmaCopy(startPal, BG_PALETTE, 256*2);
    dmaCopy(substartBitmap, bgGetGfxPtr(bgSub), 256*256);
    dmaCopy(substartPal, BG_PALETTE_SUB, 256*2);

    for(;;){
        scanKeys();
        if(keysDown() & KEY_START)
        {
            printf("start\n");
            sht = mmEffectEx(&shooting);
            break;
        }
        swiWaitForVBlank();
        printf("running \n");
        oamUpdate(&oamMain);
        oamUpdate(&oamSub);
    }    
    mmEffectCancel(sht);

    
    do{
        startgame();
    }
    while(restart == true);

	mmStop();
    return 0;
}


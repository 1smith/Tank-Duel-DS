#ifndef __shell_H__
#define __shell_H__

#include "entity.h"


using namespace std;


class shell : public entity{
	public:

		shell();
		~shell();
		int firedByMan;
		void draw(camera* c, u16* gfx, int oam);

};

#endif
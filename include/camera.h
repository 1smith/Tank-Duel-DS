#ifndef __camera_H__
#define __camera_H__

#include <nds.h>
#include <math.h>


template <class T>
struct MathVector2D {
    T x;
    T y;
};

using namespace std;


class camera{
	public:

		camera();
		~camera();

		MathVector2D<float> position;
		MathVector2D<float> size;
		void move(float x, float y);

};

#endif
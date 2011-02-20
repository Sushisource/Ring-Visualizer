#pragma once
#define TRAIL_LENGTH 100 //Defines array size of trail
#define TRAIL_WIDTH 1 //Note trail width is twice this number
#define AGE_CONST 1 //Defines how old particles get every step
#define MAX_AGE 100 //max particle age
#include "cinder/Vector.h"
#include <vector>

class NoteRay
{
public:
	//Constructor takes initial position and velocity
	NoteRay(ci::Vec2f iPos, ci::Vec2f iVel, int i);
	~NoteRay(void);
	void update(); //tells the particle to update and draw itself
	bool isdead; //if true, this particle should be deleted
	int idx; //Location in note array
	ci::Vec2f vel; //veloctiy vector
private:
	void nVel(); //velocity updater
	void nPos(); //position updater (apply velocity)
	void nAge(); //age updater
	void render(); //render note
	void renderTrail(); //render trail

	ci::Vec2f posAr[TRAIL_LENGTH]; //position array, note [0] is always current loc	
	float age; //age		
};


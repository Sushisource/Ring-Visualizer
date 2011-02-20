#include "NoteRay.h"
#include "cinder/gl/gl.h"
#include "cinder/Perlin.h"
using namespace ci;
Perlin perl(2);

NoteRay::NoteRay(Vec2f iPos, Vec2f iVel, int indx)
{
	//First we'll fill the trail with the initial position
	for (int x=0; x < TRAIL_LENGTH; x++)
		posAr[x]=iPos;

	vel = iVel;
	age = 0;
	isdead = false;
	idx = indx;
}

void NoteRay::update()
{
	nVel();
	nPos();
	nAge();
	render();
	renderTrail();
}

void NoteRay::nVel()
{
	vel += perl.dfBm(posAr[0])/100;
}

void NoteRay::nPos()
{
	// Shift the positions in the array like a trail
	for( int i = TRAIL_LENGTH - 1; i > 0; i-- ) {
		posAr[i] = posAr[i-1];
	}
	// Set the initial location
	posAr[0] += vel;
}

void NoteRay::nAge()
{
	age+=AGE_CONST;
	if (age>MAX_AGE)
		isdead=true;
}

void NoteRay::render()
{	
	float rad = TRAIL_WIDTH * vel.lengthSquared()/2;
	gl::drawSolidCircle(posAr[0],rad);
}

void NoteRay::renderTrail()
{	
	glBegin( GL_QUADS );
	//Disired effect: increasing "pixelization"?
	bool on = true;
	for( int i = 0; i < TRAIL_LENGTH - 2; i+=2 ) {
		if(on)
		{
			glColor4d(1,1,1,i/TRAIL_LENGTH);
			glVertex2f(posAr[i]+Vec2f(-TRAIL_WIDTH,-TRAIL_WIDTH));
			glVertex2f(posAr[i]+Vec2f(TRAIL_WIDTH,TRAIL_WIDTH));
			glVertex2f(posAr[i+1]+Vec2f(-TRAIL_WIDTH,-TRAIL_WIDTH));
			glVertex2f(posAr[i+1]+Vec2f(TRAIL_WIDTH,TRAIL_WIDTH));
		}
		if(i%5==true)
			on=!on;
	}
	glEnd();
}


NoteRay::~NoteRay(void)
{
}

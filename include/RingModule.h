#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "NoteRay.h"
#include "Resources.h"
#include <list>
//Radius of the whole ring
#define CIRCLE_RADIUS 180
//Number of metaballs passed to shader
#define NUM_MBALLS 40
//We skip some low frequency bins in our threshold search due to their offset
#define SKIP_LOW_F_BINS 15
using namespace ci;

class RingModule
{
public:	
	//Constructor. Takes size of FFT array
	RingModule(int dataSize, gl::GlslProg noteray);//, gl::GlslProg metaballs);
	RingModule(); //Annoying def const
	~RingModule(); //Destructor
	//Method for updating and drawing ring portion of visualizer. Accepts FFT data.
	void updateRing(float *freqData, Vec2f center, int wWidth, int wHeight);
private:	
	NoteRay **currentlyNote; //Array of NoteRay pointers
	float noteThreshold; //Intesity past which a freq is considered note
	float lastAverage;
	gl::GlslProg fShader; //Shader for noterays
	//gl::GlslProg mShader; //Shader for metaballs
	int dataSiz; //Size of FFT array
	float angleScale; //Scale for how much we need to rotate b/w each data point
	float angleShift; //and a shift to rotate whole circle
	ci::Perlin perl; //Perlin noise gen
};
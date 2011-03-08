#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "Resources.h"
using namespace ci;

class SphereModule
{
public:	
	//Constructor. Takes size of FFT array. Shader.
	SphereModule(int dataSize, gl::GlslProg sphereShader);
	//Method for updating and drawing sphere portion of visualizer. Accepts FFT data.
	void updateSphere(float *freqData, Vec2f center, int wWidth, int wHeight);
private:
	int dataSiz; //Size of FFT array
	gl::GlslProg shader; //Shader
};
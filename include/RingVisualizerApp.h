#ifndef _RING_VIS
#define _RING_VIS

#include "cinder/app/AppBasic.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Input.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/PcmBuffer.h"
#include "cinder/Perlin.h"
#include "cinder/gl/Texture.h"
#include "KissFFT.h"
#include "AudioInput.h"
#include "RingModule.h"
#include "SphereModule.h"
#define FFT_DATA_SIZ BUFFER_LENGTH
#define BIN_SIZ 213
using namespace ci;
using namespace ci::app;

class RingVisualizerApp : public AppBasic {
 public:
	RingVisualizerApp();
	void prepareSettings(Settings *settings);
	void setup();
	void update();
	void draw();

 private:
	float circleRadius; //Circle radius in pixels
	float elapsedTime; //Elapsed time
	bool down;
	//audio::TrackRef mTrack;
	//audio::PcmBuffer32fRef mPcmBuffer;
	//audio::PcmBuffer32fRef lastBuffer;
	int32_t mCallbackID; //Callback id for audio input
	AudioInput mInput; //Audio input instance
	//fft
	bool fftinit;
	float * mFreqData;
	KissRef kfft;	
	gl::GlslProg bgsh; //Background shader
	gl::Texture permTexture; //Texture array for permutation table
	gl::Texture simplexTexture; //Texture array for 3d simplex noise
	GLuint simplexTextureID;
	//Ring portion of visualizer
	RingModule *ringM;
	//Sphere portion
	SphereModule *sphereM;
	//Just hiding away the audio and fft updates to make code nicer
	void updateAudio();
	void onData(float * data, int32_t size); //Audio callback
	void initPermTexture(); //Sets up lookup texture for noise	
};
#endif
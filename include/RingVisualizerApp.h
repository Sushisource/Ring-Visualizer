#ifndef _RING_VIS
#define _RING_VIS

#include "cinder/app/AppBasic.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Input.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/PcmBuffer.h"
#include "cinder/Perlin.h"
#include "cinder/gl/Texture.h"
#include "Kiss.h"
#include "AudioInput.h"
#include "RingModule.h"
#include "SphereModule.h"
#define FFT_DATA_SIZ BUFFER_LENGTH
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
	//audio::TrackRef mTrack;
	//audio::PcmBuffer32fRef mPcmBuffer;
	//audio::PcmBuffer32fRef lastBuffer;
	int32_t mCallbackID; //Callback id for audio input
	AudioInput mInput; //Audio input instance
	//fft
	bool fftinit;
	float * mFreqData;
	Kiss kfft;
	//Ring portion of visualizer
	RingModule *ringM;
	//Sphere portion
	SphereModule *sphereM;
	//Just hiding away the audio and fft updates to make code nicer
	void updateAudio();
	//Background shader
	gl::GlslProg bgsh;
	void onData(float * data, int32_t size); //Audio callback	
	void initPermTexture(GLuint *texID); //Sets up lookup texture for noise
	Vec4f *permTexture; //Texture array for permutation table
	
};
#endif
#ifndef _RING_VIS
#define _RING_VIS

#include "cinder/app/AppBasic.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Input.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/PcmBuffer.h"
#include "cinder/Perlin.h"
#include "Kiss.h"
#include "AudioInput.h"
#include "RingModule.h"
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
	audio::TrackRef mTrack;
	audio::PcmBuffer32fRef mPcmBuffer;
	audio::PcmBuffer32fRef lastBuffer;
	//Stereo mix in stuff	
	void onData(float * data, int32_t size); //Audio callback
	int32_t mCallbackID; //Callback id
	AudioInput mInput; //Audio input instance
	//fft
	bool fftinit;
	float * mFreqData;
	Kiss kfft;
	//Ring portion of visualizer
	RingModule *ringM;
	//Just hiding away the audio and fft updates to make code nicer
	void updateAudio();
};
#endif
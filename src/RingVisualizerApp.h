#ifndef _RING_VIS
#define _RING_VIS

#include "cinder/app/AppBasic.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Input.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/PcmBuffer.h"
#include "cinder/Perlin.h"
#include "Kiss.h"

using namespace ci;
using namespace ci::app;

class RingVisualizerApp : public AppBasic {
  public:
	void prepareSettings(Settings *settings);
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();	
private:
	float circleRadius; //Circle radius in pixels
	audio::TrackRef mTrack;
	audio::PcmBuffer32fRef mPcmBuffer;
	audio::PcmBuffer32fRef lastBuffer;
	//fft
	bool fftinit;
	Kiss kfft;
	Font fnt;
};

#endif
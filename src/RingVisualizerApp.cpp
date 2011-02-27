#include "RingVisualizerApp.h"

void RingVisualizerApp::setup()
{
	/* Old style song load
	try	{
		audio::SourceRef trk = audio::load("M:\\Music\\Lily Allen\\It's Not Me, It's You\\08 Lily Allen - Fuck You.mp3","mp3");
		//audio::SourceRef trk = audio::load("M:\\Windowlicker.mp3","mp3");
		mTrack = audio::Output::addTrack( trk );	
	}
	catch(Exception e)	{
		printf("Error opening track %s", e.what());
	}
	//you must enable enable PCM buffering on the track to be able to call getPcmBuffer on it later
	mTrack->enablePcmBuffering( true );*/
	//Setup the fft
	kfft.setDataSize(FFT_DATA_SIZ);
	//Initalize stereo in
	mCallbackID = mInput.addCallback<RingVisualizerApp>(&RingVisualizerApp::onData, this);
	mInput.start();
	//Setup ring module. Chop off last 300 bins filled with garbage
	//Load the shaders
	gl::GlslProg n = gl::GlslProg(ci::app::loadResource(NOTERAY_V_SHADER),ci::app::loadResource( NOTERAY_SHADER ));
	gl::GlslProg m = gl::GlslProg(ci::app::loadResource(METABALL_V_SHADER),ci::app::loadResource( METABALL_SHADER ));
	ringM = new RingModule(kfft.getBinSize()-300,n,m);
	fftinit=true;
}

void RingVisualizerApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1500, 800 );
	settings->setFrameRate( 60.0f );
}

void RingVisualizerApp::update()
{	
	//updateAudio();	
}

void RingVisualizerApp::draw()
{		
	// clear out the window with black
	glClearColor( 0, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT );
	if( ! fftinit ) {
		return;
	}
	// Update Ring module
	ringM->updateRing(mFreqData,getWindowCenter(),getWindowWidth(),getWindowHeight());
}

// Called when audio in buffer is full
void RingVisualizerApp::onData(float * data, int32_t size)
{
	// Analyze data
	kfft.setData(data);
	mFreqData = kfft.getAmplitude();
}

void RingVisualizerApp::updateAudio()
{
	// Check if track is playing and has a PCM buffer available
	if (mTrack->isPlaying() && mTrack->isPcmBuffering())
	{
		// Get new buffer
		mPcmBuffer = mTrack->getPcmBuffer();
		if (mPcmBuffer && mPcmBuffer->getInterleavedData())
		{
			// Get sample count
			uint32_t mSampleCount = mPcmBuffer->getInterleavedData()->mSampleCount;			
			// Now we want to make our doublebuffer out of this frame and last frame's data
			// so we'll just copy the arrays next to each other
			float* newData = mPcmBuffer->getInterleavedData()->mData;						
			float* tBuf = new float[mSampleCount*2];
			memcpy(tBuf,newData,sizeof(float) * mSampleCount);
			
			if(lastBuffer)
			{
				float *oldData = lastBuffer->getInterleavedData()->mData;
				memcpy(tBuf+mSampleCount,oldData,sizeof(float) * mSampleCount);				
			}
		
			if (mSampleCount > 0)
			{
				// Initialize analyzer, if needed
				if (!fftinit)
				{
					fftinit = true;
					kfft.setDataSize(mSampleCount*2);
				}
				// Analyze data				
				if (mPcmBuffer->getInterleavedData()->mData != NULL)
					kfft.setData(tBuf);
			}
			delete tBuf;			
		}
		lastBuffer = mPcmBuffer;
	}
}

RingVisualizerApp::RingVisualizerApp(){}

CINDER_APP_BASIC( RingVisualizerApp, RendererGl )
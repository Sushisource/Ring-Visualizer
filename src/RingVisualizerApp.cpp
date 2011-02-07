#include "RingVisualizerApp.h"

void RingVisualizerApp::setup()
{
	try
	{
		audio::SourceRef trk = audio::load("M:\\Music\\Lily Allen\\It's Not Me, It's You\\08 Lily Allen - Fuck You.mp3","mp3");
		mTrack = audio::Output::addTrack( trk );	
	}
	catch(Exception e)
	{
		printf("Error %s", e.what());
	}
	//you must enable enable PCM buffering on the track to be able to call getPcmBuffer on it later
	mTrack->enablePcmBuffering( true );
	circleRadius = 200; 	

	//Setup font
	fnt = Font("Calibiri",15);

	//make sure we don't use fft before it's ready
	fftinit = false;
}

void RingVisualizerApp::mouseDown( MouseEvent event )
{	
}

void RingVisualizerApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}

void RingVisualizerApp::update()
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
			float *newData = mPcmBuffer->getInterleavedData()->mData;						
			float *tBuf = new float[mSampleCount*2];
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
					kfft = Kiss(mSampleCount*2);
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

void RingVisualizerApp::draw()
{
	Vec2f center = getWindowCenter();
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );	
	if( ! mPcmBuffer ) {
		return;
	}
	//Time for FFT STUFF
	// Get data
	float * mFreqData = kfft.getAmplitude();
	int mDataSize = kfft.getBinSize();
	mDataSize-=20;

	// Get dimensions
	float mScale = (getWindowWidth() - 20.0f) / (float)mDataSize;
	float mWindowHeight = getWindowHeight();

	// Iterate through data
	gl::color(Color(1,1,1));
	//Scale for how much we need to rotate b/w each data point, and a shift to rotate whole circle
	const float angleScale = (2 * M_PI) / mDataSize;
	const float angleShift = M_PI;
	//Average for note threshold
	float avgIntensity = 0;
	int alternate = 1;
	for (int32_t i = 0; i < mDataSize; i++) 
	{

		// Do logarithmic plotting for frequency domain
		double mLogSize = log((double)mDataSize);
		float x = (float)(log((double)i) / mLogSize) * (double)mDataSize;		
		float y = math<float>::clamp(mFreqData[i] *  (x / mDataSize) *log((double)(mDataSize - i)), 0.0f, 2.0f);
		//Plot the circle fft visualizaiton
		//If we want a circle, R should be intensity, and angle is x axis
		float c = y * 2;
		y+=circleRadius+y*circleRadius;
		avgIntensity+=y;
		float theta = ((angleScale* x*alternate+alternate*angleShift)-angleShift/2);
		Vec2f fcenter= Vec2f(y * cosf(theta), y*sinf(theta));
		fcenter += center;		
		gl::color(Color(c,c*3,c));
		gl::drawSolidCircle(fcenter,3);			
		alternate*=-1;
	}	
	avgIntensity/=mDataSize;
	gl::color(Color(.8,1,.8));
	std::ostringstream s; 
    s << avgIntensity; 
	gl::drawString(s.str(),center,Color(1,1,1),fnt);
	cinder::gl::drawStrokedCircle(center, avgIntensity);	
}

CINDER_APP_BASIC( RingVisualizerApp, RendererGl )
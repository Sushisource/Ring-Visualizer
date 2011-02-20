#include "RingVisualizerApp.h"
float lastAverage = 0;

void RingVisualizerApp::setup()
{
	try
	{
		audio::SourceRef trk = audio::load("M:\\Music\\Lily Allen\\It's Not Me, It's You\\08 Lily Allen - Fuck You.mp3","mp3");
		//audio::SourceRef trk = audio::load("M:\\Windowlicker.mp3","mp3");
		mTrack = audio::Output::addTrack( trk );	
	}
	catch(Exception e)
	{
		printf("Error %s", e.what());
	}
	//you must enable enable PCM buffering on the track to be able to call getPcmBuffer on it later
	mTrack->enablePcmBuffering( true );	

	//Setup font
	fnt = Font("Calibiri",15);

	//make sure we don't use fft before it's ready
	fftinit = false;
	//If this starts out low... kablooey
	noteThreshold = 9000;
	currentlyNote = NULL;
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

void RingVisualizerApp::draw()
{
	Vec2f center = getWindowCenter();
	// clear out the window with black
	glClearColor( 0, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT );
	if( ! mPcmBuffer ) {
		return;
	}
	//Time for FFT STUFF
	// Get data
	float * mFreqData = kfft.getAmplitude();
	int mDataSize = kfft.getBinSize();
	mDataSize-=20;
	if(currentlyNote == NULL) 
	{
		currentlyNote = new bool[mDataSize];
		std::fill_n(currentlyNote,mDataSize,false);
	}

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
	for (int i = 0; i < mDataSize; i++) 
	{

		// Do logarithmic plotting for frequency domain
		double mLogSize = log((double)mDataSize);
		float x = (float)(log((double)i) / mLogSize) * (double)mDataSize;				
		float y = math<float>::clamp(mFreqData[i] *  (x / mDataSize) *log((double)(mDataSize - i)), 0.0f, 2.0f);		 
		//Plot the circle fft visualizaiton
		//If we want a circle, R should be intensity, and angle is x axis
		float c = y * 2;
		y+=CIRCLE_RADIUS+y*CIRCLE_RADIUS;
		if(i>SKIP_LOW_F_BINS) //No DC offset! BAD DC OFFSET!
			avgIntensity+=y;
		//float theta = ((angleScale* x*alternate+alternate*angleShift)-angleShift/2);
		float theta = angleScale*x;
		Vec2f fcenter= Vec2f(y * cosf(theta), y*sinf(theta));
		fcenter += center;		
		gl::color(Color(c,c*3,c));
		if(y>noteThreshold && currentlyNote[i]==false)
		{		
			Vec2f vel = -1*(center+fcenter.inverse()).normalized() * log(y-lastAverage);
			notes.push_front(NoteRay(fcenter,vel,i));
			currentlyNote[i]=true;
		}
		gl::drawSolidCircle(fcenter,3);			
		alternate*=-1;
	}		
	avgIntensity/=mDataSize-SKIP_LOW_F_BINS;
	lastAverage = avgIntensity;
	noteThreshold = avgIntensity+avgIntensity/9;
	gl::color(Color(.8,1,.8));
	//std::ostringstream s; 
    //s << avgIntensity; 
	//gl::drawString(s.str(),center,Color(1,1,1),fnt);
	//Average circle
	gl::drawStrokedCircle(center, noteThreshold);	
	// Update noterays
	for(std::list<NoteRay>::iterator it = notes.begin(); it != notes.end();)
	{
		if (!it->isdead) {
			it->update();
			++it;
		}
		else {
			currentlyNote[it->idx] = false;
			it = notes.erase(it);			
		}
	}
}

CINDER_APP_BASIC( RingVisualizerApp, RendererGl )
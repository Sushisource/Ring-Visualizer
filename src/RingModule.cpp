#include "RingModule.h"

RingModule::RingModule(int dataSize, gl::GlslProg noteray, gl::GlslProg metaballs)
{
	//If this starts out low... kablooey
	noteThreshold = 9000;
	lastAverage = 0;	
	//Init font
	//fnt = Font("Calibiri",15);
	//Setup note Array
	dataSiz = dataSize;
	currentlyNote = new NoteRay *[dataSize];
	memset(currentlyNote,0,dataSize*sizeof(NoteRay *));
	//Setup shader refs
	fShader = noteray;
	mShader = metaballs;
	//Setup polar conversion consts
	angleShift = 3*M_PI/2;
	angleScale = (M_PI) / dataSiz;
	perl = ci::Perlin(3);
}

RingModule::RingModule(){}

RingModule::~RingModule()
{
	delete currentlyNote;	
}

void RingModule::updateRing(float *freqData, Vec2f center, int wWidth, int wHeight)
{
	// Get dimensions
	float mScale = (wWidth - 20.0f) / (float)dataSiz;
	float mWindowHeight = wHeight;	
	//Average for note threshold
	float avgIntensity = 0;
	//Update noterays
	fShader.bind();
	fShader.uniform("lightpos",Vec3f(center,10.0f));
	for(int i = 0; i < dataSiz; i++)
	{
		NoteRay *curr = currentlyNote[i];
		if(curr == NULL) continue;
		if (!curr->isdead)
			curr->update();
		else {
			delete currentlyNote[i];
			currentlyNote[i]=NULL;
		}
	}
	fShader.unbind();
	//Setup metaball arr
	Vec3f *ballArr = new Vec3f[NUM_MBALLS];
	ballArr[50]=Vec3f(center,10);
	int mod = floor((float)dataSiz/NUM_MBALLS);
	bool alternate = true;
	for (int i = 0; i < dataSiz; i++) 
	{
		// Do logarithmic plotting for frequency domain
		double mLogSize = log((double)dataSiz);
		float x = (log((double)i) / mLogSize) * dataSiz;				
		float y = math<float>::clamp(freqData[i]*(x / dataSiz)*log((double)(dataSiz - i)), 0.0f, 2.0f);		 
		//Plot the circle fft visualizaiton
		//If we want a circle, R should be intensity, and angle is x axis
		float c = y * 2;
		y+=CIRCLE_RADIUS+y*CIRCLE_RADIUS;
		if(i>SKIP_LOW_F_BINS) //No DC offset! BAD DC OFFSET!
			avgIntensity+=y;
		if(alternate)
			x=-1*x;
		float theta = ((angleScale* x)-angleShift);		
		Vec2f fcenter= Vec2f(y * cosf(theta), y*sinf(theta));
		Vec2f nRayPos = fcenter.limited(CIRCLE_RADIUS);
		fcenter += center;		
		gl::color(Color(c,c*3,c));
		if(y>noteThreshold && currentlyNote[i]==NULL)
		{		
			Vec2f vel = -1*(center+fcenter.inverse()).normalized() * log(y-lastAverage);
			currentlyNote[i]= new NoteRay(nRayPos+center,vel,i,&perl);
		}
		gl::drawSolidCircle(fcenter,3);	
		//Toss in a metaball
		ballArr[i%mod] = Vec3f(fcenter,2);
		alternate=!alternate;
	}		
	avgIntensity/=dataSiz-SKIP_LOW_F_BINS;
	lastAverage = avgIntensity;
	noteThreshold = avgIntensity+avgIntensity/9;

	gl::color(Color(1,1,1));
	gl::drawStrokedCircle(center,noteThreshold);
	
	//Draw quad with metaballs
	mShader.bind();
	mShader.uniform("balls", ballArr, NUM_MBALLS);	
	glBindFragDataLocation(mShader.getHandle(),0,"FragColor");
	gl::drawSolidRect(Rectf(100,100,700,700));
	mShader.unbind();
}
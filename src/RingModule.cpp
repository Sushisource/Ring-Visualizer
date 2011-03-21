#include "RingModule.h"

RingModule::RingModule(int dataSize, gl::GlslProg noteray)
{
	//If this starts out low... kablooey
	noteThreshold = 9000;
	lastAverage = 0;
	//Setup note Array
	dataSiz = dataSize;
	currentlyNote = new NoteRay *[dataSize];
	memset(currentlyNote,0,dataSize*sizeof(NoteRay *));
	//Grab shaders
	fShader = noteray;
	//mShader = metaballs;
	//Setup polar conversion consts
	angleShift = 3*M_PI/2;
	angleScale = (M_PI) / dataSiz;
	perl = ci::Perlin(4);
}

RingModule::RingModule(){}

RingModule::~RingModule()
{
	delete []currentlyNote;	
}

void RingModule::updateRing(float *freqData, Vec2f center, int wWidth, int wHeight)
{
	glDisable(GL_LIGHTING);
	perl.setSeed(clock());
	// Get dimensions
	float mScale = (wWidth - 20.0f) / (float)dataSiz;
	float mWindowHeight = wHeight;	
	//Average for note threshold
	float avgIntensity = 0;	

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
		float trueTheta = (angleScale *x);
		if(alternate)
			x=-1*x;
		float theta = ((angleScale* x)-angleShift);
		Vec2f fcenter= Vec2f(y * cosf(theta), y*sinf(theta));
		Vec2f nRayPos = fcenter.limited(CIRCLE_RADIUS);
		fcenter += center;		
		gl::color(ColorA(c,c*3,c,c*3));
		//Note that although y my already be a note, it will be overwritten
		if(y>noteThreshold && !(currentlyNote[i]!=NULL && currentlyNote[i]->age < 30))
		{
			Vec2f vel = -1*(center+fcenter.inverse()).normalized() * log(y-lastAverage);
			if(currentlyNote[i] != NULL)
				delete currentlyNote[i]; //Some cleanup
			currentlyNote[i]= new NoteRay(nRayPos+center,vel,i,&perl,nRayPos-center);
		}
		//gl::drawSolidCircle(fcenter,3);
		alternate=!alternate;
	}		
	avgIntensity/=dataSiz-SKIP_LOW_F_BINS;
	lastAverage = avgIntensity;
	noteThreshold = avgIntensity+avgIntensity/9;
	
	//Update noterays
	fShader.bind();
	fShader.uniform("lightpos",Vec3f(center,10.0f));
	noteRayUpdater(currentlyNote,dataSiz);
	fShader.unbind();
	
	//Draw quad with metaballs
	/** Currently unused for performance
	mShader.bind();
	mShader.uniform("balls", ballArr, NUM_MBALLS);
	mShader.uniform("lightpos", Vec3f(center,10.0f));
	gl::drawSolidRect(Rectf(0,0,wWidth,wHeight));	
	mShader.unbind();
	*/
}

void RingModule::noteRayUpdater(NoteRay **rays, int num)
{
	for(int i = 0; i < num; i++)
	{
		NoteRay *curr = rays[i];
		if(curr == NULL) continue;
		if (!curr->isdead)
			curr->update();
		else {
			delete rays[i];
			rays[i]=NULL;
		}
	}
}
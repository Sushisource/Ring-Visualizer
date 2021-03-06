#include "RingVisualizerApp.h"

//These are Ken Perlin's proposed gradients for 3D noise.
int grad3[16][3] = {{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
                   {1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
                   {1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0}, // 12 cube edges
                   {1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}}; // 4 more to make 16
//Permutation table
int perm[256]= {151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

unsigned char simplex4[][4] = {{0,64,128,192},{0,64,192,128},{0,0,0,0},
  {0,128,192,64},{0,0,0,0},{0,0,0,0},{0,0,0,0},{64,128,192,0},
  {0,128,64,192},{0,0,0,0},{0,192,64,128},{0,192,128,64},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{64,192,128,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {64,128,0,192},{0,0,0,0},{64,192,0,128},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{128,192,0,64},{128,192,64,0},
  {64,0,128,192},{64,0,192,128},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{128,0,192,64},{0,0,0,0},{128,64,192,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {128,0,64,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {192,0,64,128},{192,0,128,64},{0,0,0,0},{192,64,128,0},
  {128,64,0,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {192,64,0,128},{0,0,0,0},{192,128,0,64},{192,128,64,0}};

void RingVisualizerApp::setup()
{
	elapsedTime = 0;
	//Setup the fft
	kfft = Kiss::create(FFT_DATA_SIZ);
	//Turn on some opengl stuff
	gl::enableAlphaBlending();
	gl::enableDepthWrite();
	gl::enableDepthRead();
	glClearColor( 0, 0, 0, 1 );
	//Initalize stereo in
	mCallbackID = mInput.addCallback<RingVisualizerApp>(&RingVisualizerApp::onData, this);
	mInput.start();	
	//Setup ring module.
	//Load the shaders
	gl::GlslProg n = gl::GlslProg(ci::app::loadResource(NOTERAY_V_SHADER),ci::app::loadResource( NOTERAY_SHADER ));	
	ringM = new RingModule(BIN_SIZ,n);
	//Setup sphere module
	//Load shader
	gl::GlslProg ss = gl::GlslProg(ci::app::loadResource(SPHERE_V_SHADER),ci::app::loadResource( SPHERE_SHADER ));
	sphereM = new SphereModule(BIN_SIZ,ss);
	//Our own BG shader
	bgsh = gl::GlslProg(ci::app::loadResource(BG_V_SHADER), ci::app::loadResource( BG_SHADER ));
	initPermTexture();
	fftinit=true;
}

void RingVisualizerApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1000, 700 );
	settings->setFrameRate( 50.0f );
}

void RingVisualizerApp::update()
{	
	//updateAudio();	
}

void RingVisualizerApp::draw()
{			
	// clear out the window with black	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  );
	if( ! fftinit ) {
		return;
	}
	//Get common params
	Vec2f winCenter = getWindowCenter();
	int w = getWindowWidth();
	int h = getWindowHeight();	
	//Update sphere
	sphereM->updateSphere(mFreqData,winCenter,w,h);
	//Update Ring module
	ringM->updateRing(mFreqData,winCenter,w,h);
	//Draw the background
	bgsh.bind();	
	bgsh.uniform("permTexture",0); //tex unit 0
	bgsh.uniform("simplexTexture",1); //tex unit 1
	bgsh.uniform("wCenter",getWindowCenter());
	//Calculate a weighted mean of the spectrogram
	double lsum = 0;
	double hsum = 0;
	double sum = 0;
	for(int i = 0; i<BIN_SIZ; i++)
	{
		sum += mFreqData[i];
		if (i < BIN_SIZ/3)
			lsum += mFreqData[i];
		else
			hsum += mFreqData[i];
	}
	//Update our animation track
	elapsedTime += (hsum/lsum)/2000;
	bgsh.uniform("time", elapsedTime);
	bgsh.uniform("lowAmp",(float)lsum);
	bgsh.uniform("hiAmp",(float)hsum);
	gl::drawSolidRect(Rectf(0,0,w,h),true);
	bgsh.unbind();
}

// Called when audio in buffer is full
void RingVisualizerApp::onData(float * data, int32_t size)
{
	for(int i=0; i < size; i++)
	{
		//TODO: Parameterize volume
		data[i] = data[i]*40;
	}
	// Analyze data
	kfft->setData(data);
	mFreqData = kfft->getAmplitude();	
}

//Loads up the textures that the background
//shader needs for simplex noise
void RingVisualizerApp::initPermTexture()
{	
	unsigned char* pixels = new unsigned char[256*256*4];
	for(int i = 0; i<256; i++)
		for(int j = 0; j<256; j++) 
		{
			int offset = (i*256+j)*4;
			char value = perm[(j+perm[i]) & 0xFF];
			pixels[offset] = grad3[value & 0x0F][0] * 64 + 64;   // Gradient x
			pixels[offset+1] = grad3[value & 0x0F][1] * 64 + 64; // Gradient y
			pixels[offset+2] = grad3[value & 0x0F][2] * 64 + 64; // Gradient z
			pixels[offset+3] = value;                     // Permuted index
		}
	permTexture = gl::Texture(pixels,GL_RGBA,256,256);
	permTexture.bind(0); //Bind to texunit 0
	//Now setup the simplex 1d texture
	glActiveTexture(GL_TEXTURE1); // Activate a different texture unit (unit 1)
	glGenTextures(1, &simplexTextureID); // Generate a unique texture ID
	glBindTexture(GL_TEXTURE_1D, simplexTextureID); // Bind the texture to texture unit 1
	// GLFW texture loading functions won't work here - we need GL_NEAREST lookup.
	glTexImage1D( GL_TEXTURE_1D, 0, GL_RGBA, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, simplex4 );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	//glActiveTexture(GL_TEXTURE0); // Switch active texture unit back to 0 again
	delete[] pixels;
}

RingVisualizerApp::RingVisualizerApp(){}

CINDER_APP_BASIC( RingVisualizerApp, RendererGl )
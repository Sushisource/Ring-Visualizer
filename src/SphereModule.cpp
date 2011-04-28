#include "SphereModule.h"
GLfloat mat_emission[]		= { 0.1f, 0.2f, 0.3f, 0.0f };

SphereModule::SphereModule(int dataSize, gl::GlslProg sphereShader)
{
	dataSiz = dataSize;
	shader = sphereShader;
	perl = Perlin(3);
}

void SphereModule::updateSphere(float *freqData, Vec2f center, int wWidth, int wHeight)
{
	//Need to turn on lighting
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	gl::pushMatrices();
	gl::translate(center); //Move the model into the screen center

	//Lighting stuff --------------------------------------
	GLfloat light_position[] = {200, 0, 500, 1};
	GLfloat light_position2[] = {-200, 0, 500, 1};
	glLightfv( GL_LIGHT0, GL_POSITION,light_position);
	glLightfv( GL_LIGHT1, GL_POSITION,light_position2);
	float Al[4] = {0.2f, 0.2f, 0.2f, 1.0f };
	glLightfv( GL_LIGHT0, GL_AMBIENT, Al );
	glLightfv( GL_LIGHT1, GL_AMBIENT, Al );
	float Dl[4] = {1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv( GL_LIGHT0, GL_DIFFUSE, Dl );	
	glLightfv( GL_LIGHT1, GL_DIFFUSE, Dl );	
	float Sl[4] = {1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv( GL_LIGHT0, GL_SPECULAR, Sl );
	glLightfv( GL_LIGHT1, GL_SPECULAR, Sl );
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f );
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f );
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00002f );
	glLightf( GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f );
	glLightf( GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f );
	glLightf( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.00002f );
	//-----------------------------------------------------	

	gl::pushMatrices();
	gl::rotate(Vec3f(90,0,90)); //Rotate the sphere

	//Materials -------------------------------------------
	ColorA color(1,1,1,1);
	glMaterialfv( GL_FRONT, GL_DIFFUSE,	color);
	//glMaterialfv( GL_FRONT, GL_EMISSION, mat_emission );
	//-----------------------------------------------------

	//Load shaders, pass freq data, and draw
	shader.bind();
	shader.uniform("freqData",freqData,dataSiz);
	float chAvs[2] = {1,1};
	shader.uniform("channelAvgs",chAvs,2);
	gl::drawSphere(Vec3f::zero(),174,124);	
	shader.unbind();	
	gl::popMatrices();
	gl::popMatrices();

	//Now turn off the lighting again
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}
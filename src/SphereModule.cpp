#include "SphereModule.h"

SphereModule::SphereModule(int dataSize, gl::GlslProg sphereShader)
{
	dataSiz = dataSize;
	shader = sphereShader;
}

void SphereModule::updateSphere(float *freqData, Vec2f center, int wWidth, int wHeight)
{
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	
	GLfloat light_position[] = { center.x, center.y-50, 200, 1};
	glLightfv( GL_LIGHT0, GL_POSITION,light_position);
	float Al[4] = {0.2f, 0.2f, 0.3f, 1.0f };
	glLightfv( GL_LIGHT0, GL_AMBIENT, Al );
	float Dl[4] = {1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv( GL_LIGHT0, GL_DIFFUSE, Dl );	
	float Sl[4] = {1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv( GL_LIGHT0, GL_SPECULAR, Sl );
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f );
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f );
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0002f );
	ColorA color(1,1,1,1);
	glMaterialfv( GL_FRONT, GL_DIFFUSE,	color);
	shader.bind();
	gl::drawSphere(Vec3f(center,0),100,128);
	shader.unbind();
}
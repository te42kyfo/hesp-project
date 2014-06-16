#include "sdl_gl.hpp"

#include <vector>
#include <iostream>

using namespace std;

void SdlGl::initDrawParticles() {
	glShadeModel( GL_FLAT );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glDisable(GL_DEPTH_TEST);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );

	sphere_program = loadShader( "./vis/sphere.vert", "./vis/sphere.frag" );
}

void SdlGl::drawParticles(float* px, float* py, float* pz, float* radius, size_t particleCount,
						  float x1, float y1, float z1, float x2, float y2, float z2) {


	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	vector<GLfloat> vertices =
		{ -1.0, -1.0, 0.0,
		  1.0, -1.0, 0.0,
		  1.0, 1.0, 0.0,
		  -1.0, 1.0, 0.0};

	vector<GLubyte> indices  =
		{ 0, 1, 3,
		  1, 2, 3 };



	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data() );

	float xscale = 2.0/(x2-x1)*0.95;
	float yscale = 2.0/(y2-y1)*0.95;
	float zscale = 2.0/(z2-z1)*0.95;


	glUseProgram(sphere_program);

	for(size_t i = 0; i < particleCount; i++) {
		glLoadIdentity();

		glTranslatef( -1.0, -1.0, 0.0);
		glScalef(xscale, yscale, zscale);
		glTranslatef( px[i], py[i], pz[i]);
		glTranslatef( -x1, -y1, 0.0);
		glScalef( radius[i]/xscale, radius[i]/yscale, 0.0);


		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, indices.data() );
	}

	glUseProgram(0);

	glLoadIdentity();
	glTranslatef( -1.0, -1.0, 0.0);
	glScalef(xscale, yscale, zscale);
	glTranslatef( -x1, -y1, 0.0);
	glBegin(GL_LINE_STRIP);
	glVertex2f( x1, y1);
	glVertex2f( x2, y1);
	glVertex2f( x2, y2);
	glVertex2f( x1, y2);
	glVertex2f( x1, y1);
	glEnd();



	glDisableClientState(GL_VERTEX_ARRAY);
}

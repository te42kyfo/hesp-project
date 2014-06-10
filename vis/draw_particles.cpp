#include "sdl_gl.hpp"

#include <vector>

using namespace std;

void SdlGl::initDrawParticles() {
	glShadeModel( GL_FLAT );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glDisable(GL_DEPTH_TEST);
}

void SdlGl::drawParticles(float* px, float* py, float* pz, size_t particleCount) {
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();

	vector<GLfloat> vertices = 	{ 0.0,    0.0, 0.0,
								  0.87,   0.5, 0.0,
								  0.0,    1.0, 0.0,
								  -0.87,  0.5, 0.0,
								  -0.87, -0.5, 0.0,
								  0.0,   -1.0, 0.0,
								  0.87,  -0.5, 0.0};

	vector<GLubyte> indices  =
		{ 0, 1, 2,
		  0, 2, 3,
		  0, 3, 4,
		  0, 4, 5,
		  0, 5, 6,
		  0, 6, 1
		};



	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data() );

	for(size_t i = 0; i < particleCount; i++) {
		glLoadIdentity();

		//		glScalef(0.2, 0.2, 0.2);
		glTranslatef( px[i], py[i], pz[i]);
		glScalef(0.001, 0.001, 0.001);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, indices.data() );
	}


	glDisableClientState(GL_VERTEX_ARRAY);
}

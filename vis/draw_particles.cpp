#include "sdl_gl.hpp"

#include <vector>
#include <iostream>
#include "lodepng.h"


using namespace std;

void SdlGl::initDrawParticles() {
	glShadeModel( GL_FLAT );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glDisable(GL_DEPTH_TEST);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );

	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	glGenTextures(1, &cubeTexture );
	glBindTexture( GL_TEXTURE_CUBE_MAP, cubeTexture );


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
	//  glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

	string basename( "vis/cube_beach/" );
	for( auto it : vector<pair< GLenum, string>>
		{ {GL_TEXTURE_CUBE_MAP_POSITIVE_X, "posx"},
		  {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "negx"},
		  {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "posy"},
		  {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "negy"},
		  {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "posz"},
		  {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "negz"}}) {
		std::vector<unsigned char> image;

		unsigned int texWidth, texHeight;

		auto error = lodepng::decode( image, texWidth, texHeight,
									  basename + it.second + ".png", LCT_RGB, 8 );
		if( error ) {
			cout << "lodepng error: " << lodepng_error_text(error) << ": "
				 << basename + it.second + ".png" << "\n";
		}

		glTexImage2D( it.first, 0, GL_RGB8, texWidth, texHeight, 0, GL_RGB,
					  GL_UNSIGNED_BYTE, image.data());
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	sphere_program = loadShader( "./vis/sphere.vert", "./vis/sphere.frag" );
}

void SdlGl::drawParticles(float* px, float* py, float* pz, float* radius, size_t particleCount,
						  float x1, float y1, float z1, float x2, float y2, float z2) {

	glClear( GL_COLOR_BUFFER_BIT );
	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);

	float xscale = 2.0/(x2-x1)*0.95;
	float yscale = 2.0/(y2-y1)*0.95;
	float zscale = 2.0/(z2-z1)*0.95;

	glColor4f(1.0, 1.0, 1.0, 1.0);

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


	GLint texUloc = glGetUniformLocation(sphere_program, "tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_CUBE_MAP,  cubeTexture);
	glUniform1i( texUloc, 0);
	//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


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


	glUseProgram(sphere_program);

	for(size_t i = 0; i < particleCount; i++) {
		glLoadIdentity();

		glTranslatef( -1.0, -1.0, 0.0);
		glScalef(xscale, yscale, zscale);
		glTranslatef( px[i], py[i], pz[i]);
		glTranslatef( -x1, -y1, 0.0);
		glScalef( 1.5*radius[i]/xscale, 1.5*radius[i]/yscale, 0.0);


		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, indices.data() );
	}

	glDisableClientState(GL_VERTEX_ARRAY);
}

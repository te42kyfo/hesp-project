#include <vector>
#include <iostream>

#include <cmath>
#include "sdl_gl.hpp"


using namespace std;



void SdlGl::initDrawSlice(char* argv0) {

	perspective = 0;

	glShadeModel( GL_FLAT );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glDisable(GL_DEPTH_TEST);

	string basename( argv0 );
	basename.erase( basename.find_last_of( "/" )  );
	if( basename.empty() ) basename = ".";

	color_program = loadShader( basename +"/vis/color.vert", basename + "/vis/color.frag" );

}



void SdlGl::drawSlice(float* slice_ptr, int width, int height) {

	frame_number++;

    glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();


	cout << (float) -width/height << "\n";

	vector<GLfloat> vertices = { (float) -width/height, -1.0, 0.0,
								 (float) width/height, -1.0, 0.0,
								 (float) -width/height,  1.0, 0.0,
								 (float) width/height,  1.0, 0.0 };

	vector<GLfloat> texCoords = { 0.0, 0.0,
								  1.0, 0.0,
								  0.0, 1.0,
								  1.0, 1.0};

	vector<GLubyte> indices  = { 0, 1, 2,
								 1, 2, 3 };




	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture( GL_TEXTURE_2D, texId);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB,
				  width, height, 0,
				  GL_RGB, GL_FLOAT,
				  slice_ptr);



	GLint texUloc = glGetUniformLocation(color_program, "tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D,  texId);
    glUniform1i( texUloc, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	glUseProgram(color_program);



	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices.data() );
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords.data() );

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, indices.data() );

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDeleteTextures(1, &texId);

	glFlush();
	glLoadIdentity();
}

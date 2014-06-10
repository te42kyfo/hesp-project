#include "sdl_gl.hpp"

void SdlGl::initDrawParticles() {
	glShadeModel( GL_FLAT );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glDisable(GL_DEPTH_TEST);
}

void SdlGl::drawParticles(float* px, float* py, float* pz, size_t particleCount) {
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();


}

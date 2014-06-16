#ifndef SDL_GL_HPP
#define SDL_GL_HPP

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

class SdlGl {
public:
	std::string readShaderFile( const std::string fileName);
	GLuint loadShader(std::string vshader, std::string fshader);

	void setViewport(int width, int height);

	void SDL_die( std::string error ) ;
	void initDisplay();


	void initDrawSlice();
	void drawSlice(float* slice_ptr, size_t width, size_t height);

	void initDrawParticles();
	void drawParticles(float* px, float* py, float* pz, float* radius, size_t particleCount,
					   float x1, float y1, float z1, float x2, float y2, float z2);

    SDL_Window* window;
	SDL_GLContext  gl_context;
	unsigned int frame_number = 0;

	// drawSlice
	GLuint color_program;
	GLuint sphere_program;


};

#endif

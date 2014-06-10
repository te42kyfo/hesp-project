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
	void drawParticles(float* px, float* py, float* pz, size_t particleCount);

    SDL_Window* window;
	SDL_GLContext  gl_context;
	unsigned int frame_number = 0;

	// drawSlice
	GLuint color_program;


};

#endif

#include <iostream>
#include "sdl_gl.hpp"


using namespace std;

void SdlGl::setViewport( int width, int height ) {
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho( (float) -width/height, (float) width/height, -1, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


string SdlGl::readShaderFile(const string fileName) {
	try {
		std::ifstream t( fileName.c_str() );
		return std::string( (std::istreambuf_iterator<char>(t)),
							std::istreambuf_iterator<char>());
	} catch( std::exception& e) {
		std::cout << fileName << " - " << "readShaderFile: " << e.what() << "\n";
        return string();
    }
}

GLuint SdlGl::loadShader(string vshader, string fshader) {
    GLcharARB log[5000];
    GLsizei length;

    string vertex_shader = readShaderFile( vshader );
    string fragment_shader = readShaderFile( fshader );
    char const * my_fragment_shader_source = fragment_shader.c_str();
    char const * my_vertex_shader_source = vertex_shader.c_str();


    GLuint program = glCreateProgramObjectARB();
    GLuint vertex = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    GLuint fragment = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    glShaderSourceARB( vertex, 1, &my_vertex_shader_source, NULL);
    glShaderSourceARB( fragment, 1, &my_fragment_shader_source, NULL);
    glCompileShader( vertex);
    glCompileShader( fragment);
    glAttachObjectARB( program, vertex);
    glAttachObjectARB( program, fragment);
    glLinkProgramARB( program);

    glGetInfoLogARB( vertex, 5000, &length, log);
    if( length > 0) std::cout << "Vertex Shader: "  << log << "\n";
    glGetInfoLogARB( fragment, 5000, &length, log);
    if( length > 0) std::cout << "Fragment Shader: " << log << "\n";
    glGetInfoLogARB( program, 5000, &length, log);
    if( length > 0) std::cout << "Progrgam: " << log << "\n";

	return program;
}

void SdlGl::SDL_die( string error ) {
	cout << "Fatal error in " << error << ": " << SDL_GetError() << "\n";
	exit(EXIT_FAILURE);
}


void SdlGl::initDisplay() {

	if( SDL_Init(SDL_INIT_VIDEO) == -1) SDL_die( "SDL_Init" );

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);


    window = SDL_CreateWindow
		( "HEAT", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, 800, 600,
		  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if( window == nullptr) SDL_die("SDL_CreateWindow");

	gl_context = SDL_GL_CreateContext( window );
	if( gl_context == nullptr) SDL_die( "SDL_GL_CreateContext");

	SDL_GL_SetSwapInterval(1);
	glewInit();
	std::cout << glGetString(GL_VENDOR) << "\n";
	std::cout << glGetString(GL_RENDERER) << "\n";
	std::cout << glGetString(GL_VERSION) << "\n";
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

}

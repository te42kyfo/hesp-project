#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <cstdio>
#include <random>
#include <algorithm>


#include "vis/sdl_gl.hpp"
#include "simulation.hpp"
#include "parfile_reader.hpp"
#include "dtime.hpp"

using namespace std;





int main(int argc, char *argv[]) {


	if(argc < 2) {
		std::cerr << "Not enough parameters\n Usage: "
				  << argv[0] << "<parameter file>\n";
		exit(1);
	}

	ParfileReader params(argv[1]);

	Simulation	sim( params, argv[0] );

	size_t screenWidth =  800;
	size_t screenHeight = 600;

	SdlGl vis;
	vis.initDisplay();
	vis.initDrawSlice( argv[0] );
	vis.setViewport(screenWidth, screenHeight);

	double x1,y1,z1,x2,y2,z2;
	params.initDoubleList( { {"x_min", x1}, {"y_min", y1}, {"z_min", z1},
							 {"x_max", x2}, {"y_max", y2}, {"z_max", z2} });



	SDL_Event e;
	bool quit = false;

	while (!quit){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT){
				quit = true;
			}
			if(e.type == SDL_WINDOWEVENT) {
				if( e.window.event == SDL_WINDOWEVENT_RESIZED) {
					screenWidth = e.window.data1;
					screenHeight = e.window.data2;
					vis.setViewport(screenWidth, screenHeight);
				}
			}
			if (e.type == SDL_KEYDOWN){
				switch (e.key.keysym.sym){
				case SDLK_q:
				case SDLK_ESCAPE:
					quit = true;
					break;
				default:
					break;
				}
			}

		}



		double t1 = dtime();
		for( size_t i = 0; i< 40; i++) {
			sim.step();
		}
		double t2 = dtime();

		cout << fixed << (t2-t1)*1000 << " ";

		sim.render( screenWidth, screenHeight);
		double t3 = dtime();
		cout << fixed << (t3-t2)*1000 << " ";

		vis.drawSlice( sim.image.host().data(), screenWidth, screenHeight );
		double t4 = dtime();
		cout << fixed << (t4-t3)*1000 << "\n";

		SDL_GL_SwapWindow( vis.window);
	}


	SDL_Quit();
}

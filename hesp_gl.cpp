#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <cstdio>
#include <random>
#include <algorithm>
#include <sys/time.h>

#include "vis/sdl_gl.hpp"
#include "simulation.hpp"
#include "parfile_reader.hpp"

using namespace std;


double dtime() {
	double tseconds = 0;
	struct timeval t;
	gettimeofday( &t, NULL);
	tseconds = (double) t.tv_sec + (double) t.tv_usec*1.0e-6;
	return tseconds;
}


int main(int argc, char *argv[]) {


	if(argc < 2) {
		std::cerr << "Not enough parameters\n Usage: "
				  << argv[0] << "<parameter file>\n";
		exit(1);
	}

	ParfileReader params(argv[1]);

	Simulation	sim( params );

	SdlGl vis;
	vis.initDisplay();
	vis.initDrawParticles();
	vis.setViewport(800, 600);

	double x1,y1,z1,x2,y2,z2;
	params.initDoubleList( { {"x_min", x1}, {"y_min", y1}, {"z_min", z1},
							 {"x_max", x2}, {"y_max", y2}, {"z_max", z2} });



	SDL_Event e;
	bool quit = false;
	double frame_time = dtime();
	int iterations = 1;

	while (!quit){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT){
				quit = true;
			}
			if(e.type == SDL_WINDOWEVENT) {
				if( e.window.event == SDL_WINDOWEVENT_RESIZED) {
					vis.setViewport( e.window.data1, e.window.data2 );
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

		double now = dtime();

		double delta = now-frame_time;
		//		std::cout << " " << 1.0/ ( delta ) << " ";

		frame_time = now;

		if( delta < 0.02) iterations ++;
		if( delta > 0.03) iterations --;

		if( iterations == 0) iterations = 1;
		//std::cout << iterations << "\n";

		for( int i = 0; i < iterations; i++) {
			sim.step();
		}

		sim.copyDown();

		vis.drawParticles( sim.pos.x.host().data(),
						   sim.pos.y.host().data(),
						   sim.pos.z.host().data(),
						   sim.radius.host().data(),
						   sim.pos.z.host().size(),
						   x1, y1, z1, x2, y2, z2 );

		SDL_GL_SwapWindow( vis.window);
	}


	SDL_Quit();
}

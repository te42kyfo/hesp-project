#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <cstdio>

#include <sys/time.h>

#include "vis/sdl_gl.hpp"


using namespace std;


double dtime() {
	double tseconds = 0;
	struct timeval t;
	gettimeofday( &t, NULL);
	tseconds = (double) t.tv_sec + (double) t.tv_usec*1.0e-6;
	return tseconds;
}


void advance( vector<float>& px, vector<float>& py, vector<float>& pz)  {
	static size_t frame_counter = 0;

	for( size_t i = 0; i < px.size(); i++) {
		px[i] = sin(frame_counter*(i%10)/1000.0);
		py[i] = sin(frame_counter*(i%13)/1000.0);
		pz[i] = sin(frame_counter*(i%17)/1000.0);
	}

	frame_counter++;
}


int main(int argc, char *argv[]) {

	size_t N = 1000;

	vector<float> px(N);
	vector<float> py(N);
	vector<float> pz(N);




	SdlGl vis;
	vis.initDisplay();
	vis.initDrawParticles();
	vis.setViewport(800, 600);



	SDL_Event e;
	bool quit = false;
	double frame_time = dtime();

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

		std::cout << "\r" << 1.0/ ( (now-frame_time) );

		frame_time = now;

		vis.drawParticles( px.data(), py.data(), pz.data(), px.size() );

		SDL_GL_SwapWindow( vis.window);
	}


	SDL_Quit();
}

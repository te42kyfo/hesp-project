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
		px[i] = px[i]*0.8 + 0.2*sin(frame_counter*(i%100)/1000.0 + sin(pz[i]));
		py[i] = py[i]*0.8 + 0.2*sin(frame_counter*(i%103)/1000.0 + sin(px[i]));
		pz[i] = pz[i]*0.8 + 0.2*sin(frame_counter*(i%107)/1000.0 + sin(py[i]));
	}

	frame_counter++;
}


int main(int argc, char *argv[]) {

	size_t N = 10000;

	vector<float> px(N);
	vector<float> py(N);
	vector<float> pz(N);

	generate(px.begin(), px.end(), [](){ return (rand() % 1000) /500.0-1.0;} );
	generate(py.begin(), py.end(), [](){ return (rand() % 1000) /500.0-1.0;} );
	generate(pz.begin(), pz.end(), [](){ return (rand() % 1000) /500.0-1.0;} );




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

		advance( px, py, pz);
		vis.drawParticles( px.data(), py.data(), pz.data(), px.size() );

		SDL_GL_SwapWindow( vis.window);
	}


	SDL_Quit();
}

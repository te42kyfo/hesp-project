#include <iostream>
#include <sys/time.h>

#include "simulation.hpp"

using namespace std;

double dtime() {
  double tseconds = 0;
  struct timeval t;
  gettimeofday( &t, NULL);
  tseconds = (double) t.tv_sec + (double) t.tv_usec*1.0e-6;
  return tseconds;
}

int main(int argc, char** argv) {

	Simulation sim;
	sim.init();



	for(size_t i = 0; i < 100;i++) {
		sim.step();
	}

	sim.copyDown();





}


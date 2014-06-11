#include <iostream>
#include <sys/time.h>

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

int main(int argc, char** argv) {

	if(argc < 2) {
		std::cerr << "Not enough parameters\n Usage: "
				  << argv[0] << "<parameter file>\n";
		exit(1);
	}


	ParfileReader params(argv[1]);


	Simulation	sim(1024);


	for(size_t i = 0; i < 100;i++) {
		sim.step();
	}





	sim.copyDown();





}


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

	double time_end = params.getDouble( "time_end" );
	double dt = params.getDouble( "timestep_length" );


	Simulation	sim( params, argv[0] );


	double t = 0;
	size_t step = 0;
	while( t < time_end ) {
		if( step % params.getInt("part_out_freq") == 0) {
			ofstream outputFile( params.getString("part_out_name_base")
								 + to_string(step) + ".out" );
			sim.writeASCII( outputFile );
		}
		if( step % params.getInt("vtk_out_freq") == 0) {
			ofstream outputFile( params.getString("vtk_out_name_base")
								 + to_string(step) + ".vtk");
			sim.writeVTK( outputFile );
		}
		sim.step();
		t += dt;
		step++;
	}

}


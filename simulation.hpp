#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "ocl.hpp"
#include "parfile_reader.hpp"

#include "real.hpp"

class Simulation {
public:
	Simulation(ParfileReader& params);

	void readInputFile(std::string filename);

	void step();
	void copyDown();

	void writeASCII(std::ostream& outputStream);
	void writeVTK(std::ostream& outputStream);

	unsigned int particleCount;
	double dt;
	double sigma;
	double epsilon;
	unsigned int cl_workgroup_1dsize;


	cl_kernel update_velocities_kernel;
	cl_kernel update_positions_kernel;
	OCLv3Buffer<real> pos;
	OCLv3Buffer<real> vel;
	OCLv3Buffer<real> force;
	OCLBuffer<real> mass;


	OCL ocl;
};

#endif //SIMULATION_HPP

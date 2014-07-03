#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "ocl.hpp"
#include "parfile_reader.hpp"

#include "real.hpp"

class Simulation {
public:
	Simulation(ParfileReader& params, char* argv0);

	void readInputFile(std::string filename);

	void step();
	void copyDown();

	void render(size_t width, size_t height);

	void writeASCII(std::ostream& outputStream);
	void writeVTK(std::ostream& outputStream);


	unsigned int particleCount;
	double particle_mass;
	double gas_stiffness;
	double rest_density;
	double radius;
	double dt;


	unsigned int cl_workgroup_1dsize;

	int reflect_x, reflect_y, reflect_z;

	cl_kernel update_velocities_kernel;
	cl_kernel update_positions_kernel;
	cl_kernel update_quantities_kernel;

	cl_kernel density_field_kernel;
	cl_kernel raymarch_kernel;

	OCLv3Buffer<real> pos;
	OCLv3Buffer<real> vel;
	OCLv3Buffer<real> force;
	OCLBuffer<real> density;
	OCLBuffer<real> pressure;


	OCLBuffer<real> image;
	OCLBuffer<real> density_field;




	double x1, y1, z1;
	double x2, y2, z2;

	unsigned int nx, ny, nz;


	OCL ocl;
};

#endif //SIMULATION_HPP

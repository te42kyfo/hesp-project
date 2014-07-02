#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "ocl.hpp"
#include "parfile_reader.hpp"

#include "real.hpp"

class Simulation {
public:
	Simulation(ParfileReader& params, char* argv0);

	void readInputFile(std::string filename);

	void updateCellList();

	void step();
	void copyDown();

	void render(size_t width, size_t height);

	void writeASCII(std::ostream& outputStream);
	void writeVTK(std::ostream& outputStream);


	unsigned int particleCount;
	double dt, ks, kd;
	double gx, gy, gz;
	unsigned int cl_workgroup_1dsize;

	int reflect_x, reflect_y, reflect_z;

	cl_kernel update_velocities_kernel;
	cl_kernel update_positions_kernel;
	cl_kernel update_forces_kernel;

	cl_kernel reset_links_kernel;
	cl_kernel reset_cells_kernel;
	cl_kernel update_cells_kernel;

	cl_kernel density_field_kernel;
	cl_kernel raymarch_kernel;

	OCLv3Buffer<real> pos;
	OCLv3Buffer<real> vel;
	OCLv3Buffer<real> force;
	OCLv3Buffer<real> old_force;

	OCLBuffer<real> mass;
	OCLBuffer<real> radius;
	OCLBuffer<real> image;
	OCLBuffer<real> density_field;


	OCLBuffer<int> links;
	OCLBuffer<int> cells;


	double x1, y1, z1;
	double x2, y2, z2;

	unsigned int nx, ny, nz;


	OCL ocl;
};

#endif //SIMULATION_HPP

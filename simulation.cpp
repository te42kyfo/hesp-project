#include "simulation.hpp"

#include <fstream>
#include <iterator>
using namespace std;

Simulation::Simulation(ParfileReader& params) {
		ocl.init();

		dt = params.getDouble("timestep_length");
		sigma = params.getDouble("sigma");
		epsilon = params.getDouble("epsilon");

		params.initDoubleList( { {"x_min", x1}, {"y_min", y1}, {"z_min", z1},
							   {"x_max", x2}, {"y_max", y2}, {"z_max", z2},
							   {"r_cut", r_cut} });

		nx = params.getInt( "x_n" );
		ny = params.getInt( "y_n" );
		nz = params.getInt( "z_n" );


		cl_workgroup_1dsize = params.getInt("cl_workgroup_1dsize");

		update_velocities_kernel = ocl.buildKernel( "./update_velocities.cl",
													"update_velocities" );

		update_positions_kernel = ocl.buildKernel( "./update_positions.cl",
													"update_positions" );

		reset_cells_kernel = ocl.buildKernel( "./reset_cells.cl",
													"reset_cells" );

		reset_links_kernel = ocl.buildKernel( "./reset_links.cl",
											  "reset_links" );

		update_cells_kernel = ocl.buildKernel( "./update_cells.cl",
											  "update_cells" );

		readInputFile( params.getString( "part_input_file" ));
		force = ocl.v3Buffer<real>( pos.x.host().size() );
		links = ocl.buffer<int>( pos.x.host().size() );
		cells = ocl.buffer<int>( nx*nz*ny );


		ocl.copyUp( pos );
		ocl.copyUp( vel );
		ocl.copyUp( force );
		ocl.copyUp( mass );
		ocl.copyUp( links );
		ocl.copyUp( cells );
}

void Simulation::readInputFile(std::string filename) {
	ifstream infile(filename);
	if( !infile ) {
		cerr << "Error reading file " << filename << "\n";
		exit(1);
	}

	int element_count;
	infile >> element_count;

	istream_iterator<double> it(infile);

	while( infile ) {
		mass.host().push_back(   *it++ );
		pos.x.host().push_back(  *it++ );
		pos.y.host().push_back(  *it++ );
		pos.z.host().push_back(  *it++ );
		vel.x.host().push_back(  *it++ );
		vel.y.host().push_back(  *it++ );
		vel.z.host().push_back(  *it++ );
	}
}

void Simulation::step() {

	ocl.execute( reset_cells_kernel, 1,
				 { (nx*ny*nz/cl_workgroup_1dsize+1) * cl_workgroup_1dsize , 0, 0},
				 {cl_workgroup_1dsize, 0, 0},
				 (unsigned int) (nx*nz*ny), cells.device() );

	ocl.execute( reset_links_kernel, 1,
				 { (pos.x.deviceCount/cl_workgroup_1dsize+1) * cl_workgroup_1dsize , 0, 0},
				 {cl_workgroup_1dsize, 0, 0},
				 (unsigned int) (pos.x.deviceCount), links.device() );

	ocl.execute( update_cells_kernel, 1,
				 { (pos.x.deviceCount/cl_workgroup_1dsize+1) * cl_workgroup_1dsize , 0, 0},
				 {cl_workgroup_1dsize, 0, 0},
				 (unsigned int) pos.x.deviceCount,
				 cells.device(), links.device(),
				 pos.x.device(), pos.y.device(), pos.z.device(),
				 (real) x1, (real)y1, (real)z1, (real)x2, (real)y2, (real)z2,
				 (unsigned int) nx, (unsigned int) ny, (unsigned int) nz);


	ocl.execute( update_velocities_kernel, 1,
				 { (pos.x.deviceCount/cl_workgroup_1dsize+1) * cl_workgroup_1dsize , 0, 0},
				 {cl_workgroup_1dsize, 0, 0},
				 (unsigned int) pos.x.deviceCount,
				 (real) dt, (real) epsilon, (real) sigma,
				 mass.device(),
				 pos.x.device(), pos.y.device(), pos.z.device(),
				 vel.x.device(), vel.y.device(), vel.z.device(),
				 force.x.device(), force.y.device(), force.z.device(),
				 cells.device(), links.device(),
				 (real) x1, (real) y1, (real) z1, (real) x2, (real) y2, (real) z2,
				 (unsigned int) nx, (unsigned int) ny, (unsigned int) nz,
				 (real) r_cut );

	ocl.execute( update_positions_kernel, 1,
				 { (pos.x.deviceCount/cl_workgroup_1dsize+1) * cl_workgroup_1dsize , 0, 0},
				 {cl_workgroup_1dsize, 0, 0},
				 (unsigned int) pos.x.deviceCount, (real) dt,
				 pos.x.device(), pos.y.device(), pos.z.device(),
				 vel.x.device(), vel.y.device(), vel.z.device(),
				 force.x.device(), force.y.device(), force.z.device(),
				 (real) x1, (real)y1, (real)z1, (real)x2, (real)y2, (real)z2);
}

void Simulation::copyDown() {
	ocl.copyDown( pos );
	ocl.copyDown( vel );
	ocl.copyDown( force );
}

void Simulation::writeASCII( ostream& outputStream ) {
	copyDown();

	if( !outputStream ) {
		cerr << "Error writing output stream\n";
		exit(1);
	}

	for( size_t i = 0; i < pos.x.host().size(); i++) {
		if( !outputStream ) {
			cerr << "Error writing output stream\n";
			exit(1);
		}
		outputStream << fixed
					 << mass.host()[i] << " "
					 << pos.x.host()[i] << " "
					 << pos.y.host()[i] << " "
					 << pos.z.host()[i] << " "
					 << vel.x.host()[i] << " "
					 << vel.y.host()[i] << " "
					 << vel.z.host()[i] << "\n";
	}
}

void Simulation::writeVTK( ostream& outputStream ) {
	copyDown();

	if( !outputStream ) {
		cerr << "Error writing output stream\n";
		exit(1);
	}

	outputStream << "# vtk DataFile Version 4.0\n"
				 << "hesp visualization file\n"
				 << "ASCII\n"
				 << "DATASET UNSTRUCTURED_GRID\n"
				 << "POINTS " << mass.host().size() << " double\n";


	for( size_t i = 0; i < pos.x.host().size(); i++) {
		outputStream << fixed
					 << pos.x.host()[i] << " "
					 << pos.y.host()[i] << " "
					 << pos.z.host()[i] << "\n";
	}

	outputStream << "CELLS 0 0\n"
				 << "CELL_TYPES 0\n"
				 << "POINT_DATA " << mass.host().size() << "\n"
				 << "SCALARS m double\n"
				 << "LOOKUP_TABLE default\n";

	for( size_t i = 0; i < pos.x.host().size(); i++) {
		outputStream << fixed << mass.host()[i] << "\n";
	}

	outputStream << "VECTORS v double\n";

	for( size_t i = 0; i < pos.x.host().size(); i++) {
		outputStream << fixed
					 << vel.x.host()[i] << " "
					 << vel.y.host()[i] << " "
					 << vel.z.host()[i] << "\n";
	}

	if( !outputStream ) {
		cerr << "Error writing output stream\n";
		exit(1);
	}

}

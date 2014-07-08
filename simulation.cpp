#include "simulation.hpp"

#include <limits>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iterator>
#include "dtime.hpp"
using namespace std;

Simulation::Simulation(ParfileReader& params, char* argv0) {


		string basename( argv0 );
		basename.erase( basename.find_last_of( "/" )  );
		if( basename.empty() ) basename = ".";


		ocl.init(basename);

		dt = params.getDouble("timestep_length");

		particle_mass = params.getDouble("particle_mass");
		rest_density = params.getDouble("rest_density");
		gas_stiffness = params.getDouble("gas_stiffness");
		radius = params.getDouble("radius");


		params.initDoubleList( { {"x_min", x1}, {"y_min", y1}, {"z_min", z1},
			  				     {"x_max", x2}, {"y_max", y2}, {"z_max", z2} });



		cl_workgroup_1dsize = params.getInt("cl_workgroup_1dsize");

		update_velocities_kernel = ocl.buildKernel( basename + "/update_velocities.cl",
													"update_velocities" );
		update_positions_kernel = ocl.buildKernel( basename + "/update_positions.cl",
												   "update_positions" );

		density_field_kernel = ocl.buildKernel( basename + "/density_field.cl",
												"density_field" );
		raymarch_kernel = ocl.buildKernel( basename + "/raymarch.cl",
										   "raymarch" );
		update_quantities_kernel = ocl.buildKernel( basename + "/update_quantities.cl",
													"update_quantities" );


		readInputFile( params.getString( "part_input_file" ));
		force = ocl.v3Buffer<real>( pos.x.host().size() );

		density  = ocl.buffer<real>( pos.x.host().size() );
		pressure = ocl.buffer<real>( pos.x.host().size() );


		ocl.copyUp( pos );
		ocl.copyUp( vel );
		ocl.copyUp( force );
}

void Simulation::readInputFile(std::string filename) {
	ifstream infile(filename);
	if( !infile.is_open() ) {
		perror( filename.c_str() );
		exit(1);
	}

	string line;
	getline(infile, line);

	while( getline(infile, line) ) {
		istringstream line_stream(line);

		istream_iterator<double> it( line_stream );


		pos.x.host().push_back(  *it++ );
		pos.y.host().push_back(  *it++ );
		pos.z.host().push_back(  *it++ );
		vel.x.host().push_back(  *it++ );
		vel.y.host().push_back(  *it++ );
		vel.z.host().push_back(  *it++ );


	}
	if (infile.bad()) {
		perror( filename.c_str() );
	}

}

void Simulation::step() {

	ocl.execute( update_quantities_kernel, 1,
				 { (pos.x.deviceCount/cl_workgroup_1dsize+1) * cl_workgroup_1dsize , 0, 0},
				 {cl_workgroup_1dsize, 0, 0},
				 (unsigned int) pos.x.deviceCount, (real) particle_mass, (real) radius,
				 (real) gas_stiffness, (real) rest_density,
				 pos.x.device(), pos.y.device(), pos.z.device(),
				 density.device(), pressure.device(),
				 (real) x1, (real) y1, (real) z1, (real) x2, (real) y2, (real) z2);


	ocl.execute( update_velocities_kernel, 1,
				 { (pos.x.deviceCount/cl_workgroup_1dsize+1) * cl_workgroup_1dsize , 0, 0},
				 {cl_workgroup_1dsize, 0, 0},
				 (unsigned int) pos.x.deviceCount,
				 (real) dt, (real) particle_mass, (real) radius,
				 pos.x.device(), pos.y.device(), pos.z.device(),
				 vel.x.device(), vel.y.device(), vel.z.device(),
				 force.x.device(), force.y.device(), force.z.device(),
				 pressure.device(), density.device(),
				 (real) x1, (real) y1, (real) z1, (real) x2, (real) y2, (real) z2);

	ocl.execute( update_positions_kernel, 1,
				 { (pos.x.deviceCount/cl_workgroup_1dsize+1) * cl_workgroup_1dsize , 0, 0},
				 {cl_workgroup_1dsize, 0, 0},
				 (unsigned int) pos.x.deviceCount, (real) dt,
				 reflect_x,  reflect_y,  reflect_z,
				 pos.x.device(), pos.y.device(), pos.z.device(),
				 vel.x.device(), vel.y.device(), vel.z.device(),
				 force.x.device(), force.y.device(), force.z.device(),
				 (real) x1, (real)y1, (real)z1, (real)x2, (real)y2, (real)z2);
}

void Simulation::render( size_t imageWidth, size_t imageHeight) {


	double t1 = dtime();

	unsigned int xcount = 128;
	unsigned int ycount = xcount * ((y2-y1)/(x2-x1));
	unsigned int zcount = xcount * ((z2-z1)/(x2-x1));

	image.host().resize(imageWidth*imageHeight*3);
	density_field.host().resize( xcount*ycount*zcount );

	size_t local3DSize = 8;

	ocl.syncSizes( density_field );

	size_t global_x_size = (xcount/local3DSize+1)*local3DSize;
	size_t global_y_size = (ycount/local3DSize+1)*local3DSize;
	size_t global_z_size = (zcount/local3DSize+1)*local3DSize;

	ocl.execute( density_field_kernel, 3,
				 { global_x_size, global_y_size, global_z_size },
				 { local3DSize, local3DSize, local3DSize},
				 (unsigned int) pos.x.deviceCount, (real) radius,
				 pos.x.device(), pos.y.device(), pos.z.device(), density_field.device(),
				 localMemory { local3DSize*local3DSize*local3DSize *sizeof(unsigned int) },
				 xcount, ycount, zcount,
				 (real) x1, (real)y1, (real)z1, (real)x2, (real)y2, (real)z2);


	ocl.finish();
	double t2 = dtime();
	cout << " . " << (t2-t1)*1000 << " ";

	ocl.syncSizes( image );

	ocl.execute( raymarch_kernel, 2,
		     { (imageWidth/16+1)*16, (imageHeight/16+1)*16, 0 },
		     { 16, 16, 0},
		     density_field.device(),
		     cl_uint4{ xcount, ycount, zcount, 0},
		     cl_float4 {x1,y1,z1, 0.0}, cl_float4{x2, y2, z2, 0.0},
		     image.device(), (int) imageWidth, (int) imageHeight,
		     cl_float4{0.0, 0.0, -3.0, 0.0}, cl_float4{0.0, 0.0, 1.0, 2.0});

	ocl.copyDown( image );

	double t3 = dtime();
	cout << 1000*(t3-t2) << " . ";

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
				 << "POINTS " << vel.x.host().size() << " double\n";


	for( size_t i = 0; i < pos.x.host().size(); i++) {
		outputStream << fixed
					 << pos.x.host()[i] << " "
					 << pos.y.host()[i] << " "
					 << pos.z.host()[i] << "\n";
	}

	outputStream << "CELLS 0 0\n"
				 << "CELL_TYPES 0\n"
				 << "POINT_DATA " << pos.x.host().size() << "\n";
	
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

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
		ks = params.getDouble("k_s");
		kd = params.getDouble("k_d");


		params.initDoubleList( { {"x_min", x1}, {"y_min", y1}, {"z_min", z1},
			  				     {"x_max", x2}, {"y_max", y2}, {"z_max", z2},
								 {"g_x", gx}, {"g_y", gy}, {"g_z", gz} });

		nx = params.getInt( "x_n" );
		ny = params.getInt( "y_n" );
		nz = params.getInt( "z_n" );

		reflect_x = params.getInt( "reflect_x" );
		reflect_y = params.getInt( "reflect_y" );
		reflect_z = params.getInt( "reflect_z" );


		cl_workgroup_1dsize = params.getInt("cl_workgroup_1dsize");

		update_velocities_kernel = ocl.buildKernel( basename + "/update_velocities.cl",
													"update_velocities" );
		update_positions_kernel = ocl.buildKernel( basename + "/update_positions.cl",
												   "update_positions" );
		reset_cells_kernel = ocl.buildKernel( basename + "/reset_cells.cl",
											  "reset_cells" );
		reset_links_kernel = ocl.buildKernel( basename + "/reset_links.cl",
											  "reset_links" );
		update_cells_kernel = ocl.buildKernel( basename + "/update_cells.cl",
											   "update_cells" );
		density_field_kernel = ocl.buildKernel( basename + "/density_field.cl",
												"density_field" );
		raymarch_kernel = ocl.buildKernel( basename + "/raymarch.cl",
										   "raymarch" );

		readInputFile( params.getString( "part_input_file" ));
		force = ocl.v3Buffer<real>( pos.x.host().size() );
		links = ocl.buffer<int>( pos.x.host().size() );
		cells = ocl.buffer<int>( nx*nz*ny );


		ocl.copyUp( pos );
		ocl.copyUp( vel );
		ocl.copyUp( force );
		ocl.copyUp( mass );
		ocl.copyUp( radius );
		ocl.copyUp( links );
		ocl.copyUp( cells );
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

		string mass_string;
		line_stream >> mass_string;
		if( mass_string == "inf" ) {
			mass.host().push_back( std::numeric_limits<real>::infinity() );
		} else {
			mass.host().push_back( stod( mass_string ) );
		}

		istream_iterator<real> it( line_stream );

		radius.host().push_back(  *it++ );
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
				 (real) dt, (real) ks, (real) kd,
				 (real) gx, (real) gy, (real) gz,
				 reflect_x,  reflect_y,  reflect_z,
				 mass.device(), radius.device(),
				 pos.x.device(), pos.y.device(), pos.z.device(),
				 vel.x.device(), vel.y.device(), vel.z.device(),
				 force.x.device(), force.y.device(), force.z.device(),
				 cells.device(), links.device(),
				 (real) x1, (real) y1, (real) z1, (real) x2, (real) y2, (real) z2,
				 (unsigned int) nx, (unsigned int) ny, (unsigned int) nz);

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

	unsigned int xcount = 64;
	unsigned int ycount = xcount * ((y2-y1)/(x2-x1));
	unsigned int zcount = xcount * ((z2-z1)/(x2-x1));

	image.host().resize(imageWidth*imageHeight*3);
	density_field.host().resize( xcount*ycount*zcount );


	ocl.syncSizes( density_field );

	size_t global_x_size = (xcount/8+1)*8;
	size_t global_y_size = (ycount/8+1)*8;
	size_t global_z_size = (zcount/8+1)*8;

	ocl.execute( density_field_kernel, 3,
				 { global_x_size, global_y_size, global_z_size },
				 { 8, 8, 8},
				 (unsigned int) pos.x.deviceCount,
				 pos.x.device(), pos.y.device(), pos.z.device(), density_field.device(),
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
				 xcount, ycount, zcount,
				 (real) x1, (real)y1, (real)z1, (real)x2, (real)y2, (real)z2,
				 image.device(), (unsigned int) imageWidth, (unsigned int) imageHeight,
				 cl_float4{0.0, 0.0, -4.0, 0.0}, cl_float4{0.0, 0.0, 1.0, 2.0});

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
					 << mass.host()[i] << " "
					 << radius.host()[i] << " "
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

	outputStream << "SCALARS r double\n"
				 << "LOOKUP_TABLE default\n";

	for( size_t i = 0; i < pos.x.host().size(); i++) {
		outputStream << fixed << radius.host()[i] << "\n";
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

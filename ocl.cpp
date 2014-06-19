#include "ocl.hpp"
#include "errors.hpp"
#include <exception>


using namespace std;

void cl_check( cl_int code ) {
	if( code != CL_SUCCESS) {
		cerr << "OpenCL error code " << code
				  << ": " << clErrorString(code) << "\n";
		exit(1);
	}
}

string clGetPlatformInfoString( cl_platform_info info_id, cl_platform_id platform_id ) {
	size_t return_size = 0;
	cl_check( clGetPlatformInfo( platform_id, info_id, 0, nullptr, &return_size));
	char return_value[return_size];

	cl_check( clGetPlatformInfo( platform_id, info_id, return_size,
								 return_value, NULL));
	return string(return_value);
}


void OCL::init() {
	init( "." );
}

void OCL::init(string basename) {
	cl_platform_id ids[5];
	cl_uint num_platforms = 0;

	cl_check( clGetPlatformIDs( 5, ids, &num_platforms) );
	cl_platform_id chosen_id = ids[0];

	for( size_t id = 0; id < num_platforms; id++) {
		cout << to_string(id) << ": "
				  << clGetPlatformInfoString( CL_PLATFORM_VENDOR, ids[id]) << "\n"
				  << clGetPlatformInfoString( CL_PLATFORM_NAME, ids[id]) << "\n\n";
	}

	if( num_platforms > 1) {
		cout << "Choose Platform: ";
		size_t chosen_idx = 0;
		cin >> chosen_idx;
		chosen_id = ids[chosen_idx];
	}

	cl_int error;

	cl_check( clGetDeviceIDs(chosen_id, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL) );
	context = clCreateContext(0, 1, & device, NULL, NULL, &error);
	cl_check( error );

	queue = clCreateCommandQueue(context, device, 0, &error);
	cl_check( error );
	this->basename = basename;
}

cl_kernel OCL::buildKernel ( string filename, string kernel_name) {
	string source;
	try {
		ifstream t( filename.c_str() );
		if( !t ) {
			cout  << "buildKernel: Could not open " << filename << "\n";
			exit(1);
		}
		source = string( (istreambuf_iterator<char>(t)),
						 istreambuf_iterator<char>());
	} catch( exception& e) {
		exit(1);
	}

	cl_int error = CL_SUCCESS;

	const char* char_source = source.c_str();

	cl_program program = clCreateProgramWithSource( context, 1, &char_source,
													NULL, &error);
	cl_check(error);

	string options( "-I");
	options += basename;


	clBuildProgram(program, 0, NULL, options.c_str(), NULL, NULL);



	cl_build_status build_status = CL_BUILD_SUCCESS;

	clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_STATUS,
						   sizeof(cl_build_status), &build_status, nullptr);


	if( build_status != CL_BUILD_SUCCESS) {

		size_t logsize = 0;
		cl_check( clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG,
										 0, 0, &logsize));
		char log[logsize];

		cl_check( clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG,
										 logsize, log, nullptr));

		cout << "Build log of file " << filename <<  " :\n"  << log << "\n";
	}

	cl_kernel kernel = clCreateKernel(program, kernel_name.c_str(), &error);
	cl_check(error);

	return kernel;
}

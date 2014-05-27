#include <CL/opencl.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

void cl_check( cl_int code ) {
	if( code != CL_SUCCESS) {
		std::cerr << "OpenCL error, code " << code << "\n";
		exit(1);
	}
}

std::string clGetPlatformInfoString( cl_platform_info info_id, cl_platform_id platform_id ) {
	size_t return_size = 0;
	cl_check( clGetPlatformInfo( platform_id, info_id, 0, nullptr, &return_size));
	char return_value[return_size];

	cl_check( clGetPlatformInfo( platform_id, info_id, return_size,
								 return_value, NULL));
	return std::string(return_value);
}




class OCL {
public:
	void init() {
		cl_platform_id ids[5];
		cl_uint num_platforms = 0;

		cl_check( clGetPlatformIDs( 5, ids, &num_platforms) );

		cl_platform_id chosen_id = ids[0];

		for( size_t id = 0; id < num_platforms; id++) {
			std::cout << std::to_string(id) << ": "
					  << clGetPlatformInfoString( CL_PLATFORM_VENDOR, ids[id]) << "\n"
					  << clGetPlatformInfoString( CL_PLATFORM_NAME, ids[id]) << "\n\n";
		}

		if( num_platforms > 1) {
			std::cout << "Choose Platform: ";
			size_t chosen_idx = 0;
			std::cin >> chosen_idx;
			chosen_id = ids[chosen_idx];
		}

		cl_int error;
		cl_device_id device;
		cl_check( clGetDeviceIDs(chosen_id, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL) );
		context = clCreateContext(0, 1, & device, NULL, NULL, &error);
		cl_check( error );

		queue = clCreateCommandQueue(context, device, 0, &error);
		cl_check( error );
	}

	cl_kernel kernel (std::string filename, std::string kernel_name) {
		std::string source;
		try {
			std::ifstream t( filename.c_str() );
			source = std::string( (std::istreambuf_iterator<char>(t)),
								  std::istreambuf_iterator<char>());
		} catch( std::exception& e) {
			std::cout << filename << " - " << "readShaderFile: " << e.what() << "\n";
			exit(1);
		}

		cl_int error = CL_SUCCESS;

		const char* char_source = source.c_str();

		cl_program program = clCreateProgramWithSource( context, 1, &char_source,
														NULL, &error);
		cl_check(error);

		cl_check( clBuildProgram(program, 0, NULL, NULL, NULL, NULL));

		cl_kernel kernel = clCreateKernel(program, kernel_name.c_str(), &error);
		cl_check(error);
		return kernel;
	}

private:
	cl_context context;
	cl_command_queue queue;
};

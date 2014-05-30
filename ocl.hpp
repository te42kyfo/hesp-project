#include <CL/opencl.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>

#include "errors.hpp"
#include "v3.h"

void cl_check( cl_int code ) {
	if( code != CL_SUCCESS) {
		std::cerr << "OpenCL error code " << code
				  << ": " << clErrorString(code) << "\n";
		//		exit(1);
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


template<class T>
struct OCLBuffer {
	std::unique_ptr<T> host_mem;
	cl_mem device_mem;
	T* host() { return host_mem.get(); };
	cl_mem& device() { return device_mem; };

	size_t elements;
};

template<class T>
struct OCLv3Buffer {
	OCLBuffer<T> x;
	OCLBuffer<T> y;
	OCLBuffer<T> z;
	v3ptr getV3ptr() {
		return v3ptr{ (T*) x.device(), (T*) y.device(), (T*) z.device() };
	}

};


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

		cl_check( clGetDeviceIDs(chosen_id, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL) );
		context = clCreateContext(0, 1, & device, NULL, NULL, &error);
		cl_check( error );

		queue = clCreateCommandQueue(context, device, 0, &error);
		cl_check( error );
	}

	cl_kernel buildKernel (std::string filename, std::string kernel_name) {
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



		cl_build_status build_status = CL_BUILD_SUCCESS;

		cl_check( clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_STATUS,
										 sizeof(cl_build_status), &build_status, nullptr));


		if( build_status != CL_BUILD_SUCCESS) {

			size_t logsize = 0;
			cl_check( clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG,
											 0, 0, &logsize));
			char log[logsize];

			cl_check( clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG,
											 logsize, log, nullptr));

			std::cout << "Build log: " << log << "\n";
		}

		cl_kernel kernel = clCreateKernel(program, kernel_name.c_str(), &error);
		cl_check(error);

		return kernel;
	}

	template<class T>
	OCLBuffer<T> buffer( size_t elements ) {

		OCLBuffer<T> result;
		result.elements = elements;
		result.host_mem = std::unique_ptr<T>(new T[ elements ] ) ;
		cl_int error;
		result.device_mem = clCreateBuffer( context, CL_MEM_READ_WRITE, elements*sizeof(T),
											nullptr, &error);
		cl_check(error);
		return result;
	}

	template<class T>
	void copyUp( OCLBuffer<T>& buffer) {
		cl_check( clEnqueueWriteBuffer( queue, buffer.device_mem, CL_TRUE, 0,
										sizeof(T) * buffer.elements,
										buffer.host_mem.get(), 0, NULL, NULL));
	}

	template<class T>
	void copyDown( OCLBuffer<T>& buffer) {
		cl_check( clEnqueueReadBuffer( queue, buffer.device_mem, CL_TRUE, 0,
									   sizeof(T) * buffer.elements,
									   buffer.host_mem.get(), 0, NULL, NULL));
	}



	template<class T>
	OCLv3Buffer<T> v3Buffer( size_t elements ) {
		OCLv3Buffer<T> result;
		result.x = this->buffer<T>( elements );
		result.y = this->buffer<T>( elements );
		result.z = this->buffer<T>( elements );
		return result;
	}

	template<class T>
	void copyUp( OCLv3Buffer<T>& buffer) {
		this->copyUp( buffer.x );
		this->copyUp( buffer.y );
		this->copyUp( buffer.z );

	}

	template<class T>
	void copyDown( OCLv3Buffer<T>& buffer) {
		this->copyDown( buffer.x );
		this->copyDown( buffer.y );
		this->copyDown( buffer.z );
	}


private:
	void execute_t( size_t argument_index, cl_kernel kernel, size_t dim,
				  std::vector<size_t> global_size, std::vector<size_t> local_size) {

		cl_check( clEnqueueNDRangeKernel( queue, kernel, dim, NULL,
										  global_size.data(), local_size.data(),
										  0, NULL, NULL)) ;

	}

	template<typename T, typename ... Args>
	void execute_t( size_t argument_index, cl_kernel kernel, size_t dim,
				  std::vector<size_t> global_size, std::vector<size_t> local_size,
				  T argument, Args... args) {
		cl_check( clSetKernelArg( kernel, argument_index, sizeof(T), argument) );

		execute_t( argument_index+1, kernel, dim, global_size, local_size, args...);
	}
public:
	template<typename ... Args>
	void execute( cl_kernel kernel, size_t dim,
				  std::vector<size_t> global_size, std::vector<size_t> local_size,
				  Args... args) {

		execute_t( 0, kernel, dim, global_size, local_size, args...);
	}


	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
};

#ifndef OCL_HPP
#define OCL_HPP

#include <CL/opencl.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include "v3.h"

void cl_check( cl_int code );

std::string clGetPlatformInfoString( cl_platform_info info_id, cl_platform_id platform_id );


template<class T>
struct OCLBuffer {
	std::vector<T> host_mem;
	cl_mem device_mem = nullptr;
	std::vector<T>& host() { return host_mem; };
	cl_mem device() { return device_mem; };

	size_t deviceCount = 0;
};

template<class T>
struct OCLv3Buffer {
	OCLBuffer<T> x;
	OCLBuffer<T> y;
	OCLBuffer<T> z;
};


class OCL {
public:
	void init();

	cl_kernel buildKernel (std::string filename, std::string kernel_name);

	template<class T>
	OCLBuffer<T> buffer( size_t elements ) {
		OCLBuffer<T> result;
		if( elements == 0) return result;

		result.deviceCount = elements;
		result.host_mem = std::vector<T>(elements);
		cl_int error;
		result.device_mem = clCreateBuffer( context, CL_MEM_READ_WRITE, elements*sizeof(T),
											nullptr, &error);
		cl_check(error);
		return result;

	}

	template<class T>
	void copyUp( OCLBuffer<T>& buffer) {
		if( buffer.host_mem.size() == 0) return;

		if( buffer.device_mem == nullptr) {
			buffer.deviceCount = buffer.host_mem.size();
			cl_int error;
			buffer.device_mem = clCreateBuffer( context, CL_MEM_READ_WRITE,
												buffer.deviceCount*sizeof(T),
												nullptr, &error);
			cl_check(error);
		}
		cl_check( clEnqueueWriteBuffer( queue, buffer.device_mem, CL_TRUE, 0,
										sizeof(T) * buffer.deviceCount,
										buffer.host().data(), 0, NULL, NULL));
	}

	template<class T>
	void copyDown( OCLBuffer<T>& buffer) {
		cl_check( clEnqueueReadBuffer( queue, buffer.device_mem, CL_TRUE, 0,
									   sizeof(T) * buffer.deviceCount,
									   buffer.host().data(), 0, NULL, NULL));
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
		cl_int err =  clSetKernelArg( kernel, argument_index, sizeof(T), &argument);
		if( err != CL_SUCCESS) {
			std::cerr << "Argument index " << argument_index << " " << argument << "\n";
			cl_check(err);
		}

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

#endif

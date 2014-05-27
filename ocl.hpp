#include <CL/opencl.h>
#include <vector>
#include <string>

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
	void choosePlatform() {
		cl_platform_id ids[5];
		cl_uint num_platforms = 0;

		cl_check( clGetPlatformIDs( 5, ids, &num_platforms) );

		cl_platform_ids chosen_id = ids[0];

		if( num_platforms > 1) {
			for( size_t id = 0; id < num_platforms; id++) {
				std::cout << std::to_string(id) << ": "
						  << clGetPlatformInfoString( CL_PLATFORM_VENDOR, ids[id]) << "\n"
						  << clGetPlatformInfoString( CL_PLATFORM_NAME, ids[id]) << "\n";
			}
			size_t chosen_index = 0;
			cin >> chosen_index;
			chosen_id = ids[chosen_idx];
		}




		auto platforms = clGetPlatforms();

		size_t

		if( platforms.size > 1) {
			cout << "Choose a platform:\n\n";
			for( auto it : getCLPlatforms() ) {
				cout << it << "\n\n";
			}
		}
	}

	void createContext() {

	}

private:
	cl_context context;
}

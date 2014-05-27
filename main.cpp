#include <iostream>
#include "ocl.hpp"


using namespace std;

int main(int argc, char** argv) {

	OCL ocl;

	ocl.init();


	cl_kernel add = ocl.kernel( "./add.cl", "add");
	//CL_Buffer input = ocl.buffer(1000 * sizeof(double) );
	//CL_Buffer output = ocl.buffer(1000 * sizeof(double) );

	//input.host[231] = 23;
	//ocl.copyUp(input);


	//	ocl.execute( add, 1, {1000, 0, 0}, {64, 0, 0}, 1000, input, output);


	//	ocl.copyDown( output );



	

}


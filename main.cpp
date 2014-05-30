#include <iostream>
#include "ocl.hpp"
#include <sys/time.h>

using namespace std;

double dtime() {
  double tseconds = 0;
  struct timeval t;
  gettimeofday( &t, NULL);
  tseconds = (double) t.tv_sec + (double) t.tv_usec*1.0e-6;
  return tseconds;
}

int main(int argc, char** argv) {

	OCL ocl;
	ocl.init();

	size_t const N = 1024;
	size_t const iters = 100000;

	float dt = 0.0001;

	cl_kernel update_velocities_kernel = ocl.buildKernel( "./update_velocities.cl",
														  "update_velocities" );

	auto pos = ocl.v3Buffer<float>( N );
	auto vel = ocl.v3Buffer<float>( N );
	auto force = ocl.v3Buffer<float>( N );
	auto mass = ocl.buffer<float>( N );

	double start = dtime();

	ocl.copyUp( pos );
	ocl.copyUp( vel );
	ocl.copyUp( force );
	ocl.copyUp( mass );

	v3ptr pos_ptr = pos.getV3ptr();
	v3ptr vel_ptr = vel.getV3ptr();
	v3ptr force_ptr = force.getV3ptr();

	ocl.execute( update_velocities_kernel, 1,
				 {1024, 1024, 0},
				 {1024, 2, 0},
				 N, dt, pos.getV3ptr(), vel.getV3ptr(), force.getV3ptr() );


	ocl.copyDown( pos );
	ocl.copyDown( vel );
	ocl.copyDown( force );
	clFinish(ocl.queue);

	double end = dtime();

	double walltime = end-start;

	std::cout << walltime << "s, " << iters*N / walltime * 1.0e-9 <<  "GB/s\n";




}


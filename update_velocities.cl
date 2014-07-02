#include "real.hpp"

__kernel void update_velocities( const unsigned int N, const real dt,
								 global real * m,
								 global real * vx, global real * vy, global real * vz,
								 global real * fx, global real * fy, global real * fz,
								 global real * ofx, global real * ofy, global real * ofz) {


	const int globalid = get_global_id(0);
	if( globalid >= N ) return;


	vx[globalid] += (fx[globalid] + ofx[globalid]) * dt * 0.5 / m[globalid];
	vy[globalid] += (fy[globalid] + ofy[globalid]) * dt * 0.5 / m[globalid];
	vz[globalid] += (fz[globalid] + ofz[globalid]) * dt * 0.5 / m[globalid];

}

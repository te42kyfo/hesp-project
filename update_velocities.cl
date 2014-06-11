#include "real.hpp"

__kernel void update_velocities( const unsigned int N,
								 const real dt, const real epsilon, const real sigma,
								 global real * m,
								 global real * px, global real * py, global real * pz,
								 global real * vx, global real * vy, global real * vz,
								 global real * fx, global real * fy, global real * fz,
								 real x1, real y1, real z1,
								 real x2, real y2, real z2) {


	const int globalid = get_global_id(0);
	if( globalid >= N ) return;

	real new_force_x = 0;
	real new_force_y = 0;
	real new_force_z = 0;

	for( unsigned int i = 0; i < N; i++) {
		if( i == globalid ) continue;

		real dx = px[globalid] - px[i];
		real dy = py[globalid] - py[i];
		real dz = pz[globalid] - pz[i];

		if( dx > (x2-x1)*0.5 ) dx -= (x2-x1);
		if( dy > (y2-y1)*0.5 ) dy -= (y2-y1);
		if( dz > (z2-z1)*0.5 ) dz -= (z2-z1);

		if( dx < -(x2-x1)*0.5 ) dx += (x2-x1);
		if( dy < -(y2-y1)*0.5 ) dy += (y2-y1);
		if( dz < -(z2-z1)*0.5 ) dz += (z2-z1);


		real invsqlength = 1.0/ (dx*dx + dy*dy + dz*dz);
		real sigma6 = sigma*sigma*sigma;
		sigma6 = sigma6*sigma6;

		real factor =
			24.0*epsilon*invsqlength *
			sigma6*invsqlength*invsqlength*invsqlength*
			(2.0*sigma6*invsqlength*invsqlength*invsqlength-1);

		new_force_x += factor*dx;
		new_force_y += factor*dy;
		new_force_z += factor*dz;

	}


	vx[globalid] += (fx[globalid] + new_force_x) * dt * 0.5 / m[globalid];
	vy[globalid] += (fy[globalid] + new_force_y) * dt * 0.5 / m[globalid];
	vz[globalid] += (fz[globalid] + new_force_z) * dt * 0.5 / m[globalid];



	vx[globalid] *= 0.9995;
	vy[globalid] *= 0.9995;
	vz[globalid] *= 0.9995;

	fx[globalid] = new_force_x;
	fy[globalid] = new_force_y;
	fz[globalid] = new_force_z;
}



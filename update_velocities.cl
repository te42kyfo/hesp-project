#include "real.hpp"

__kernel void update_velocities( const unsigned int N,
								 const real dt, const real epsilon, const real sigma,
								 global real * m,
								 global real * px, global real * py, global real * pz,
								 global real * vx, global real * vy, global real * vz,
								 global real * fx, global real * fy, global real * fz,
								 global int* cells, global int* links,
								 const real xmin, const real ymin, const real zmin,
								 const real xmax, const real ymax, const real zmax,
								 const unsigned int nx,
								 const unsigned int ny,
								 const unsigned int nz,
								 const real r_cut) {


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

	fx[globalid] = new_force_x;
	fy[globalid] = new_force_y;
	fz[globalid] = new_force_z;
}



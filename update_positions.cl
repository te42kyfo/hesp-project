#include "real.hpp"

__kernel void update_positions ( const unsigned N, const real dt,
								 int reflect_x, int reflect_y, int reflect_z,
								 global real* px, global real* py, global real* pz,
								 global real* vx, global real* vy, global real* vz,
								 global real* fx, global real* fy, global real* fz,
								 real xmin, real ymin, real zmin,
								 real xmax, real ymax, real zmax) {

	size_t gid = get_global_id(0);
	if( gid >= N) return;

	real dt2 = 0.5*dt*dt;

	px[gid] += dt * vx[gid] + dt2 * fx[gid];
	py[gid] += dt * vy[gid] + dt2 * fy[gid];
	pz[gid] += dt * vz[gid] + dt2 * fz[gid];


	if( px[gid] < xmin ) {
		px[gid] = 2.0f * xmin - px[gid];
		vx[gid] *= -0.5;
	}

	if( px[gid] > xmax ) {
		px[gid] = 2.0f * xmax - px[gid];
		vx[gid] *= -0.5;
	}

	if( py[gid] < ymin ) {
		py[gid] = 2.0f * ymin - py[gid];
		vy[gid] *= -0.5;
	}

	if( py[gid] > ymax ) {
		py[gid] = 2.0f * ymax - py[gid];
		vy[gid] *= -0.5;
	}

	if( pz[gid] < zmin ) {
		pz[gid] = 2.0f * zmin - pz[gid];
		vz[gid] *= -0.5;
	}

	if( pz[gid] > zmax ) {
		pz[gid] = 2.0f * zmax - pz[gid];
		vz[gid] *= -0.5;
	}

/*
	real bx = (px[gid]-x1) / (x2-x1);
	real by = (py[gid]-y1) / (y2-y1);
	real bz = (pz[gid]-z1) / (z2-z1);

	if( bx >= (real) 1.0 ) px[gid] = x1+fmod(bx, (real) 1.0) * (x2-x1);
	if( by >= (real) 1.0 ) py[gid] = y1+fmod(by, (real) 1.0) * (y2-y1);
	if( bz >= (real) 1.0 ) pz[gid] = z1+fmod(bz, (real) 1.0) * (z2-z1);

	if( bx < (real) 0.0 ) px[gid] = x1+((real) 1.0-fmod(bx, (real) 1.0)) * (x2-x1);
	if( by < (real) 0.0 ) py[gid] = y1+((real) 1.0-fmod(by, (real) 1.0)) * (y2-y1);
	if( bz < (real) 0.0 ) pz[gid] = z1+((real) 1.0-fmod(bz, (real) 1.0)) * (z2-z1);

*/


}

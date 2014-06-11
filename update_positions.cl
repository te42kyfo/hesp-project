#include "real.hpp"

__kernel void update_positions ( const unsigned N, const real dt,
								 global real* px, global real* py, global real* pz,
								 global real* vx, global real* vy, global real* vz,
								 global real* fx, global real* fy, global real* fz) {

	size_t gid = get_global_id(0);
	if( gid >= N) return;

	real dt2 = 0.5*dt*dt;

	px[gid] += dt * vx[gid] + dt2 * fx[gid];
	py[gid] += dt * vy[gid] + dt2 * fy[gid];
	pz[gid] += dt * vz[gid] + dt2 * fz[gid];
}

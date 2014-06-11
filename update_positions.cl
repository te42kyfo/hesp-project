#include "real.hpp"

__kernel void update_positions ( const unsigned N, const real dt,
								 global real* px, global real* py, global real* pz,
								 global real* vx, global real* vy, global real* vz,
								 global real* fx, global real* fy, global real* fz,
								 real x1, real y1, real z1,
								 real x2, real y2, real z2) {

	size_t gid = get_global_id(0);
	if( gid >= N) return;

	real dt2 = 0.5*dt*dt;

	px[gid] += dt * vx[gid] + dt2 * fx[gid];
	py[gid] += dt * vy[gid] + dt2 * fy[gid];
	pz[gid] += dt * vz[gid] + dt2 * fz[gid];


	if( px[gid] < x1 ) px[gid] += (x2-x1);
	if( py[gid] < y1 ) py[gid] += (y2-y1);
	if( pz[gid] < z1 ) pz[gid] += (z2-z1);

	if( px[gid] > x2 ) px[gid] -= (x2-x1);
	if( py[gid] > y2 ) py[gid] -= (y2-y1);
	if( pz[gid] > z2 ) pz[gid] -= (z2-z1);


}

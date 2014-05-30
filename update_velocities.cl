#include "v3.h"

__kernel void update_velocities ( size_t N, float dt,
								  v3ptr pos,
								  v3ptr vel,
								  v3ptr forces ) {
	size_t gid = get_global_id(0);
	if( gid >= N) return;

	pos.x[gid] += dt * vel.x[gid] + 0.5 * dt*dt * forces.x[gid];
	pos.y[gid] += dt * vel.y[gid] + 0.5 * dt*dt * forces.y[gid];
	pos.z[gid] += dt * vel.z[gid] + 0.5 * dt*dt * forces.z[gid];


}


__kernel void update_velocities ( const unsigned N, const float dt,
								  global float* px, global float* py, global float* pz,
								  global float* vx, global float* vy, global float* vz,
								  global float* fx, global float* fy, global float* fz) {

	size_t gid = get_global_id(0);
	if( gid >= N) return;

	float dt2 = 0.5*dt*dt;

	px[gid] += dt * vx[gid] + dt2 * fx[gid];
	py[gid] += dt * vy[gid] + dt2 * fy[gid];
	pz[gid] += dt * vz[gid] + dt2 * fz[gid];
}


__kernel void update_velocities ( const unsigned N, const float dt,
								  global float* px, global float* py, global float* pz,
								  global float* vx, global float* vy, global float* vz,
								  global float* fx, global float* fy, global float* fz) {

	size_t gid = get_global_id(0);
	if( gid >= N) return;

	px[gid] += dt * vx[gid] + 0.5 * dt*dt * fx[gid];
	py[gid] += dt * vy[gid] + 0.5 * dt*dt * fy[gid];
	pz[gid] += dt * vz[gid] + 0.5 * dt*dt * fz[gid];


}

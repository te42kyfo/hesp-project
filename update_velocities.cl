#include "real.hpp"

__kernel void update_velocities( const unsigned int N,
				 const real dt, const real particle_mass,
				 global real * vx, global real * vy, global real * vz,
				 global real * fx, global real * fy, global real * fz,
				 global real * ofx, global real * ofy, global real * ofz) {

  const int globalid = get_global_id(0);
  if( globalid >= N ) return;

  vx[globalid] += (fx[globalid] + ofx[globalid]) * dt * 0.5 / particle_mass;
  vy[globalid] += (fy[globalid] + ofy[globalid]) * dt * 0.5 / particle_mass;
  vz[globalid] += (fz[globalid] + ofz[globalid]) * dt * 0.5 / particle_mass;

  vx[globalid] *= 0.996;
  vy[globalid] *= 0.996;
  vz[globalid] *= 0.996;
}

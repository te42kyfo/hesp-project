#include "real.hpp"

float3 spiky_gradient_kernel( const float3 distance, const real h) {
	real d = length(distance);

	real diff = h-d;
	if ( diff < 0 ) return 0.0;
	if ( d <= 0.00000001) return 0.0;
	real factor  = 1.0/ d * diff * diff / (13.0/6.0*(h*h*h));
	return distance*factor;
}

float viscosity_kernel( const float3 distance, const float h) {
	real diff = h-length(distance);
	if( diff < 0) return 0;
	return 45.0/M_PI/(h*h*h*h*h*h)*diff;
}

__kernel void update_forces( const unsigned int N,
							 const real particle_mass, const real radius,
							 const real viscosity,
							 global real * px, global real * py, global real * pz,
							 global real * vx, global real * vy, global real * vz,
							 global real * fx, global real * fy, global real * fz,
							 global real * ofx, global real * ofy, global real * ofz,
							 global real * pdensity, global real * ppressure,
							 real xmin, real ymin, real zmin,
							 real xmax, real ymax, real zmax) {


	const int globalid = get_global_id(0);
	if( globalid >= N ) return;

	float3 new_force = 0.0f;

	for( unsigned int i = 0; i < N; i++) {
		if( i != globalid ) {


			float3 distance = {px[globalid] - px[i], py[globalid] - py[i], pz[globalid] - pz[i]};

			float3 pgrad = spiky_gradient_kernel( distance, radius);
			real factor = (ppressure[globalid] + ppressure[i])*0.5 * particle_mass / pdensity[i];
			new_force += pgrad*factor;


			float3 dv = (float3) ( vx[i] - vx[globalid],
								   vy[i] - vy[globalid],
								   vz[i] - vz[globalid]);

			new_force += dv * particle_mass/pdensity[i]
				* viscosity_kernel( distance, radius ) * viscosity;


		}
  }

  new_force.y -= 100.0*particle_mass;

  ofx[globalid] = fx[globalid];
  ofy[globalid] = fy[globalid];
  ofz[globalid] = fz[globalid];

  fx[globalid] = new_force.x;
  fy[globalid] = new_force.y;
  fz[globalid] = new_force.z;
}

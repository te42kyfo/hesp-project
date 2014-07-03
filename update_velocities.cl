#include "real.hpp"

float3 spiky_gradient_kernel( const real dx, const real dy, const real dz, const real h) {
	real d = sqrt( dx*dx + dy*dy + dz*dz );

	real diff = h-d;
	if ( diff < 0 ) return 0.0;
	if ( d <= 0.00000001) return 0.0;
	real factor  = 1.0/ d * diff * diff / (13.0/6.0*(h*h*h));
	float3 result = { dx*factor, dy*factor, dz*factor };
	return result;
}

__kernel void update_velocities( const unsigned int N,
								 const real dt, const real particle_mass, const real radius,
								 global real * px, global real * py, global real * pz,
								 global real * vx, global real * vy, global real * vz,
								 global real * fx, global real * fy, global real * fz,
								 global real * ppressure, global real * pdensity,
								 real xmin, real ymin, real zmin,
								 real xmax, real ymax, real zmax) {


	const int globalid = get_global_id(0);
	if( globalid >= N ) return;



	real new_force_x = 0;
	real new_force_y = 0;
	real new_force_z = 0;


	for( unsigned int i = 0; i < N; i++) {
		if( i != globalid ) {

			real dx = px[globalid] - px[i];
			real dy = py[globalid] - py[i];
			real dz = pz[globalid] - pz[i];


			float3 pgrad = spiky_gradient_kernel( dx, dy, dz, radius);
			real factor = 	(ppressure[globalid] + ppressure[i])*0.5 *
				particle_mass / pdensity[i];

			new_force_x += pgrad.x*factor;
			new_force_y += pgrad.y*factor;
			new_force_z += pgrad.z*factor;

		}
	}

	/*
	new_force_x +=
		( spiky_gradient_kernel( px[globalid] - xmin, 0.0f, 0.0f, radius).x
		  + spiky_gradient_kernel( xmax - px[globalid], 0.0f, 0.0f, radius).x)
		* ppressure[globalid] * 10.0;

	new_force_y +=
		( spiky_gradient_kernel( 0.0f, py[globalid] - ymin, 0.0f, radius).y
		  + spiky_gradient_kernel( 0.0f, ymax - py[globalid], 0.0f, radius).y)
		* ppressure[globalid] * 10.0;

	new_force_z +=
		( spiky_gradient_kernel( 0.0f, 0.0f,  pz[globalid] - zmin, radius).z
		  + spiky_gradient_kernel( 0.0f, 0.0f, zmax - pz[globalid], radius).z)
		* ppressure[globalid] * 10.0;
		*/
	new_force_y -= 10.0*particle_mass;



	vx[globalid] += (fx[globalid] + new_force_x) * dt * 0.5 / particle_mass;
	vy[globalid] += (fy[globalid] + new_force_y) * dt * 0.5 / particle_mass;
	vz[globalid] += (fz[globalid] + new_force_z) * dt * 0.5 / particle_mass;

	vx[globalid] *= 0.996;
	vy[globalid] *= 0.996;
	vz[globalid] *= 0.996;

	fx[globalid] = new_force_x;
	fy[globalid] = new_force_y;
	fz[globalid] = new_force_z;
}

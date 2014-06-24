#include "real.hpp"


real default_kernel( const real dx, const real dy, const real dz, const double h)  {
	real cudiff = h*h - (dx*dx + dy*dy + dz*dz);
	if ( cudiff < 0 ) return 0.0;
	return 35.0f/32.0f/(h*h*h*h*h*h*h) *cudiff*cudiff*cudiff;
}

real spiky_kernel( const real dx, const real dy, const real dz, const double h)  {
	real d2 = (dx*dx + dy*dy + dz*dz);
	if ( h*h < d2 ) return 0.0f;
	double diff = h-sqrt(d2);
	return diff*diff*diff*0.25f/(h*h*h*h);
}



__kernel void update_quantities( const unsigned int N,
								 const real particle_mass, const real radius,
								 const real gas_stiffness, const real rest_density,
								 global real * px, global real * py, global real * pz,
								 global real * pdensity, global real* ppressure) {


	const int globalid = get_global_id(0);
	if( globalid >= N ) return;

	pdensity[globalid] = 0;
	ppressure[globalid] = 0;

	for( unsigned int i = 0; i < N; i++) {

		real dx = px[globalid] - px[i];
		real dy = py[globalid] - py[i];
		real dz = pz[globalid] - pz[i];

		real partial_density = default_kernel( dx, dy, dz, radius );
		pdensity[globalid] += partial_density* particle_mass;


		if( i != globalid ) {
			ppressure[globalid] += spiky_kernel( dy, dy, dz, radius ) * particle_mass;
		}

	}


	ppressure[globalid] = ( ppressure[globalid] - rest_density) * gas_stiffness;



}

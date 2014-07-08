#include "real.hpp"

real default_kernel( const real dx, const real dy, const real dz,
					 const real h, const real invh6)  {
	real cudiff = h*h - (dx*dx + dy*dy + dz*dz);
	if ( cudiff < 0 ) return 0.0;
	return invh6 *cudiff*cudiff*cudiff;
}

__kernel void density_field( const unsigned particleCount, real radius,
							 global real* px, global real* py, global real* pz,
							 global real* density_field, __local unsigned int* preCheck,
							 unsigned int xcount, unsigned int ycount, unsigned int zcount,
							 real xmin, real ymin, real zmin,
							 real xmax, real ymax, real zmax ) {

	radius *= 0.5;

	size_t gidx = get_global_id(0);
	size_t gidy = get_global_id(1);
	size_t gidz = get_global_id(2);

	size_t lidx = get_local_id(0);
	size_t lidy = get_local_id(1);
	size_t lidz = get_local_id(2);

	size_t lxsize = get_local_size(0);
	size_t lysize = get_local_size(1);
	size_t lzsize = get_local_size(2);
	size_t totalLocalSize = lxsize*lysize*lzsize;

	size_t localid = lidx + lidy*lxsize + lidz*lxsize*lysize;
	size_t globalid = (gidz*xcount*ycount + gidy*xcount +gidx);

	if( gidx >= xcount || gidy >= ycount || gidz >= zcount) return;

	density_field[globalid] = 0;

	const real invh6 = 35.0f/32.0f * native_recip( pown( radius, 6) );



	float cellx = xmin + (float) gidx/xcount * (xmax-xmin);
	float celly = ymin + (float) gidy/ycount * (ymax-ymin);
	float cellz = zmin + (float) gidz/zcount * (zmax-zmin);


	density_field[globalid] = 0.0f;

	for( unsigned int i = 0; i < particleCount; i++) {
		float dx = px[i] - cellx;
		float dy = py[i] - celly;
		float dz = pz[i] - cellz;


		density_field[globalid] += default_kernel( dx, dy, dz, radius, invh6);

	}



}

#include "real.hpp"

__kernel void density_field( const unsigned particleCount,
							 global real* px, global real* py, global real* pz,
							 global real* density_field, __local unsigned int* preCheck,
							 unsigned int xcount, unsigned int ycount, unsigned int zcount,
							 real xmin, real ymin, real zmin,
							 real xmax, real ymax, real zmax ) {


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

	if( gidx >= xcount || gidy >= ycount || gidz >= zcount) return;

	size_t globalid = (gidz*xcount*ycount + gidy*xcount +gidx);


	float cellx = xmin + (float) gidx/xcount * (xmax-xmin);
	float celly = ymin + (float) gidy/ycount * (ymax-ymin);
	float cellz = zmin + (float) gidz/zcount * (zmax-zmin);



	for( unsigned int i = 0; i < particleCount / totalLocalSize + 1; i++) {
		if( i*totalLocalSize + localid < particleCount) {
			float dx = px[i*totalLocalSize + localid] - cellx;
			float dy = py[i*totalLocalSize + localid] - celly;
			float dz = pz[i*totalLocalSize + localid] - cellz;
			float d = dx*dx+dy*dy+dz*dz;
			preCheck[localid] = 0;
			if( d < 20.0 ) {
				preCheck[localidy] = 1;
			}
		}
		barrier( CLK_LOCAL_MEM_FENCE );

		for( unsigned int n = 0; n < totalLocalSize; n++) {
			size_t current_idx = i*totalLocalSize + n;
			if( preCheck[0] != 0 ) {
			//			float dx = px[current_idx] - cellx;
			//		float dy = py[current_idx] - celly;
			//	float dz = pz[current_idx] - cellz;
			//	float d = dx*dx+dy*dy+dz*dz;
				density_field[globalid] = localid;
			}
		}
	}



}

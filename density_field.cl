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
	size_t globalid = (gidz*xcount*ycount + gidy*xcount +gidx);

	if( gidx >= xcount || gidy >= ycount || gidz >= zcount) return;

	density_field[globalid] = 0;

	if( gidx < xcount-1 && gidy < ycount-1 && gidz < zcount &&
		gidx > 0 && gidy > 0 && gidz > 0)  {



		float cellx = xmin + (float) gidx/xcount * (xmax-xmin);
		float celly = ymin + (float) gidy/ycount * (ymax-ymin);
		float cellz = zmin + (float) gidz/zcount * (zmax-zmin);


		density_field[globalid] = 0.0f;

		for( unsigned int i = 0; i < particleCount; i++) {
			float dx = px[i] - cellx;
			float dy = py[i] - celly;
			float dz = pz[i] - cellz;
			float d = dx*dx+dy*dy+dz*dz;
			if( d < 20.0 ) {
				density_field[globalid] += 1.0f/d;
			}
		}
	}



}
